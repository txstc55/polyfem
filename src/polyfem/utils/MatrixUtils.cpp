#include "MatrixUtils.hpp"

#include <polyfem/utils/MaybeParallelFor.hpp>
#include <polyfem/utils/Logger.hpp>

#include <igl/list_to_matrix.h>

#include <iostream>
#include <fstream>
#include <iomanip> // setprecision
#include <vector>
#include <filesystem>

void polyfem::utils::show_matrix_stats(const Eigen::MatrixXd &M)
{
	Eigen::FullPivLU<Eigen::MatrixXd> lu(M);
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(M);
	double s1 = svd.singularValues()(0);
	double s2 = svd.singularValues()(svd.singularValues().size() - 1);
	double cond = s1 / s2;

	logger().trace("----------------------------------------");
	logger().trace("-- Determinant: {}", M.determinant());
	logger().trace("-- Singular values: {} {}", s1, s2);
	logger().trace("-- Cond: {}", cond);
	logger().trace("-- Invertible: {}", lu.isInvertible());
	logger().trace("----------------------------------------");
	// logger().trace("{}", lu.solve(M) );
}

polyfem::utils::SpareMatrixCache::SpareMatrixCache(const size_t size)
	: size_(size)
{
	tmp_.resize(size_, size_);
	mat_.resize(size_, size_);
	mat_.setZero();
}

polyfem::utils::SpareMatrixCache::SpareMatrixCache(const size_t rows, const size_t cols)
	: size_(rows == cols ? rows : 0)
{
	tmp_.resize(rows, cols);
	mat_.resize(rows, cols);
	mat_.setZero();
}

polyfem::utils::SpareMatrixCache::SpareMatrixCache(const polyfem::utils::SpareMatrixCache &other)
{
	init(other);
}

void polyfem::utils::SpareMatrixCache::init(const size_t size)
{
	assert(mapping().empty() || size_ == size);

	size_ = size;
	tmp_.resize(size_, size_);
	mat_.resize(size_, size_);
	mat_.setZero();
}

void polyfem::utils::SpareMatrixCache::init(const size_t rows, const size_t cols)
{
	assert(mapping().empty());

	size_ = rows == cols ? rows : 0;
	tmp_.resize(rows, cols);
	mat_.resize(rows, cols);
	mat_.setZero();
}

void polyfem::utils::SpareMatrixCache::init(const SpareMatrixCache &other)
{
	if (main_cache_ == nullptr)
	{
		if (other.main_cache_ == nullptr)
			main_cache_ = &other;
		else
			main_cache_ = other.main_cache_;
	}
	size_ = other.size_;

	values_.resize(other.values_.size());

	tmp_.resize(other.mat_.rows(), other.mat_.cols());
	mat_.resize(other.mat_.rows(), other.mat_.cols());
	mat_.setZero();
	std::fill(values_.begin(), values_.end(), 0);
}

void polyfem::utils::SpareMatrixCache::set_zero()
{
	tmp_.setZero();
	mat_.setZero();

	std::fill(values_.begin(), values_.end(), 0);
}

void polyfem::utils::SpareMatrixCache::add_value(const int e, const int i, const int j, const double value)
{
	if (mapping().empty())
	{
		entries_.emplace_back(i, j, value);
		if (second_cache_entries_.size() <= e)
			second_cache_entries_.resize(e + 1);
		second_cache_entries_[e].emplace_back(i, j);
	}
	else
	{
		if (use_second_cache_)
		{
			if (e != current_e_)
			{
				current_e_ = e;
				current_e_index_ = 0;
			}

			values_[second_cache()[e][current_e_index_]] += value;
			current_e_index_++;
		}
		else
		{
			// mapping()[i].find(j)
			const auto &map = mapping()[i];
			bool found = false;
			for (const auto &p : map)
			{
				if (p.first == j)
				{
					assert(p.second < values_.size());
					values_[p.second] += value;
					found = true;
					break;
				}
			}
			assert(found);
		}
	}
}

void polyfem::utils::SpareMatrixCache::prune()
{
	if (mapping().empty())
	{
		tmp_.setFromTriplets(entries_.begin(), entries_.end());
		tmp_.makeCompressed();
		mat_ += tmp_;

		tmp_.setZero();
		tmp_.data().squeeze();
		mat_.makeCompressed();

		entries_.clear();

		mat_.makeCompressed();
	}
}

polyfem::StiffnessMatrix polyfem::utils::SpareMatrixCache::get_matrix(const bool compute_mapping)
{
	prune();

	if (mapping().empty())
	{
		if (compute_mapping && size_ > 0)
		{
			assert(main_cache_ == nullptr);

			values_.resize(mat_.nonZeros());
			inner_index_.resize(mat_.nonZeros());
			outer_index_.resize(mat_.rows() + 1);
			mapping_.resize(mat_.rows());

			const auto inn_ptr = mat_.innerIndexPtr();
			const auto out_ptr = mat_.outerIndexPtr();
			inner_index_.assign(inn_ptr, inn_ptr + inner_index_.size());
			outer_index_.assign(out_ptr, out_ptr + outer_index_.size());

			size_t index = 0;
			for (size_t i = 0; i < mat_.rows(); ++i)
			{

				const auto start = outer_index_[i];
				const auto end = outer_index_[i + 1];

				for (size_t ii = start; ii < end; ++ii)
				{
					const auto j = inner_index_[ii];
					auto &map = mapping_[j];
					map.emplace_back(i, index);
					++index;
				}
			}

			logger().trace("Cache computed");

			if (use_second_cache_)
			{
				second_cache_.clear();
				second_cache_.resize(second_cache_entries_.size());
				for (int e = 0; e < second_cache_entries_.size(); ++e)
				{
					for (const auto &p : second_cache_entries_[e])
					{
						const int i = p.first;
						const int j = p.second;

						const auto &map = mapping()[i];
						int index = -1;
						for (const auto &p : map)
						{
							if (p.first == j)
							{
								assert(p.second < values_.size());
								index = p.second;
								break;
							}
						}
						assert(index >= 0);

						second_cache_[e].emplace_back(index);
					}
				}

				second_cache_entries_.resize(0);

				logger().trace("Second cache computed");
			}
		}
	}
	else
	{
		assert(size_ > 0);
		const auto &outer_index = main_cache_ == nullptr ? outer_index_ : main_cache_->outer_index_;
		const auto &inner_index = main_cache_ == nullptr ? inner_index_ : main_cache_->inner_index_;
		mat_ = Eigen::Map<const StiffnessMatrix>(
			size_, size_, values_.size(), &outer_index[0], &inner_index[0], &values_[0]);

		if (use_second_cache_)
		{
			current_e_ = -1;
			current_e_index_ = -1;

			logger().trace("Using second cache");
		}
		else
			logger().trace("Using cache");
	}
	std::fill(values_.begin(), values_.end(), 0);
	return mat_;
}

polyfem::utils::SpareMatrixCache polyfem::utils::SpareMatrixCache::operator+(const SpareMatrixCache &a) const
{
	polyfem::utils::SpareMatrixCache out(a);

	if (a.mapping().empty() || mapping().empty())
	{
		out.mat_ = a.mat_ + mat_;
		if (use_second_cache_)
		{
			const size_t this_e_size = second_cache_entries_.size();
			const size_t a_e_size = a.second_cache_entries_.size();

			out.second_cache_entries_.resize(std::max(this_e_size, a_e_size));
			for (int e = 0; e < std::min(this_e_size, a_e_size); ++e)
			{
				assert(second_cache_entries_[e].size() == 0 || a.second_cache_entries_[e].size() == 0);
				out.second_cache_entries_[e].insert(out.second_cache_entries_[e].end(), second_cache_entries_[e].begin(), second_cache_entries_[e].end());
				out.second_cache_entries_[e].insert(out.second_cache_entries_[e].end(), a.second_cache_entries_[e].begin(), a.second_cache_entries_[e].end());
			}

			for (int e = std::min(this_e_size, a_e_size); e < std::max(this_e_size, a_e_size); ++e)
			{
				if (second_cache_entries_.size() < e)
					out.second_cache_entries_[e].insert(out.second_cache_entries_[e].end(), second_cache_entries_[e].begin(), second_cache_entries_[e].end());
				else
					out.second_cache_entries_[e].insert(out.second_cache_entries_[e].end(), a.second_cache_entries_[e].begin(), a.second_cache_entries_[e].end());
			}
		}
	}
	else
	{
		const auto &outer_index = main_cache_ == nullptr ? outer_index_ : main_cache_->outer_index_;
		const auto &inner_index = main_cache_ == nullptr ? inner_index_ : main_cache_->inner_index_;
		const auto &aouter_index = a.main_cache_ == nullptr ? a.outer_index_ : a.main_cache_->outer_index_;
		const auto &ainner_index = a.main_cache_ == nullptr ? a.inner_index_ : a.main_cache_->inner_index_;
		assert(ainner_index.size() == inner_index.size());
		assert(aouter_index.size() == outer_index.size());
		assert(a.values_.size() == values_.size());

		maybe_parallel_for(a.values_.size(), [&](int start, int end, int thread_id) {
			for (int i = start; i < end; ++i)
			{
				out.values_[i] = a.values_[i] + values_[i];
			}
		});
	}

	return out;
}

void polyfem::utils::SpareMatrixCache::operator+=(const SpareMatrixCache &o)
{
	if (mapping().empty() || o.mapping().empty())
	{
		mat_ += o.mat_;

		if (use_second_cache_)
		{
			const size_t this_e_size = second_cache_entries_.size();
			const size_t o_e_size = o.second_cache_entries_.size();

			second_cache_entries_.resize(std::max(this_e_size, o_e_size));
			for (int e = 0; e < o_e_size; ++e)
			{
				assert(second_cache_entries_[e].size() == 0 || o.second_cache_entries_[e].size() == 0);
				second_cache_entries_[e].insert(second_cache_entries_[e].end(), o.second_cache_entries_[e].begin(), o.second_cache_entries_[e].end());
			}
		}
	}
	else
	{
		const auto &outer_index = main_cache_ == nullptr ? outer_index_ : main_cache_->outer_index_;
		const auto &inner_index = main_cache_ == nullptr ? inner_index_ : main_cache_->inner_index_;
		const auto &oouter_index = o.main_cache_ == nullptr ? o.outer_index_ : o.main_cache_->outer_index_;
		const auto &oinner_index = o.main_cache_ == nullptr ? o.inner_index_ : o.main_cache_->inner_index_;
		assert(inner_index.size() == oinner_index.size());
		assert(outer_index.size() == oouter_index.size());
		assert(values_.size() == o.values_.size());

		maybe_parallel_for(o.values_.size(), [&](int start, int end, int thread_id) {
			for (int i = start; i < end; ++i)
			{
				values_[i] += o.values_[i];
			}
		});
	}
}

// Flatten rowwises
Eigen::VectorXd polyfem::utils::flatten(const Eigen::MatrixXd &X)
{
	Eigen::VectorXd x(X.size());
	for (int i = 0; i < X.rows(); ++i)
	{
		for (int j = 0; j < X.cols(); ++j)
		{
			x(i * X.cols() + j) = X(i, j);
		}
	}
	assert(X(0, 0) == x(0));
	assert(X.cols() <= 1 || X(0, 1) == x(1));
	return x;
}

// Unflatten rowwises, so every dim elements in x become a row.
Eigen::MatrixXd polyfem::utils::unflatten(const Eigen::VectorXd &x, int dim)
{
	assert(x.size() % dim == 0);
	Eigen::MatrixXd X(x.size() / dim, dim);
	for (int i = 0; i < x.size(); ++i)
	{
		X(i / dim, i % dim) = x(i);
	}
	assert(X(0, 0) == x(0));
	assert(X.cols() <= 1 || X(0, 1) == x(1));
	return X;
}

Eigen::SparseMatrix<double> polyfem::utils::lump_matrix(const Eigen::SparseMatrix<double> &M)
{
	std::vector<Eigen::Triplet<double>> triplets;

	for (int k = 0; k < M.outerSize(); ++k)
	{
		for (Eigen::SparseMatrix<double>::InnerIterator it(M, k); it; ++it)
		{
			triplets.emplace_back(it.row(), it.row(), it.value());
		}
	}

	Eigen::SparseMatrix<double> lumped(M.rows(), M.rows());
	lumped.setFromTriplets(triplets.begin(), triplets.end());
	lumped.makeCompressed();

	return lumped;
}
