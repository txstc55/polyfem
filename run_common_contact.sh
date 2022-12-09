cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/codimensional/mat-knives.json >> ../matrixChangeResults/matrixChange_mat-knives.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "codimensional/mat-knives.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/friction/arch.json >> ../matrixChangeResults/matrixChange_arch.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "friction/arch.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/friction/armadillo-roller.json >> ../matrixChangeResults/matrixChange_armadillo-roller.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "friction/armadillo-roller.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/friction/ball-rollers.json >> ../matrixChangeResults/matrixChange_ball-rollers.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "friction/ball-rollers.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/friction/card-house.json >> ../matrixChangeResults/matrixChange_card-house.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "friction/card-house.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/friction/high-school-physics-slopetest-mu=0.49.json >> ../matrixChangeResults/matrixChange_high-school-physics-slopetest-mu=0.49.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "friction/high-school-physics-slopetest-mu=0.49.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/friction/high-school-physics-slopetest-mu=0.50.json >> ../matrixChangeResults/matrixChange_high-school-physics-slopetest-mu=0.50.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "friction/high-school-physics-slopetest-mu=0.50.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/friction/stick-slip.json >> ../matrixChangeResults/matrixChange_stick-slip.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "friction/stick-slip.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/higher-order/golf-ball-P1.json >> ../matrixChangeResults/matrixChange_golf-ball-P1.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "higher-order/golf-ball-P1.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/higher-order/golf-ball-P2.json >> ../matrixChangeResults/matrixChange_golf-ball-P2.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "higher-order/golf-ball-P2.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/large-ratios/large-mass-ratio.json >> ../matrixChangeResults/matrixChange_large-mass-ratio.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "large-ratios/large-mass-ratio.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/large-ratios/large-stiffness-ratio.json >> ../matrixChangeResults/matrixChange_large-stiffness-ratio.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "large-ratios/large-stiffness-ratio.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/large-ratios/sphere-mat.json >> ../matrixChangeResults/matrixChange_sphere-mat.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "large-ratios/sphere-mat.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/mesh-sequence/kick.json >> ../matrixChangeResults/matrixChange_kick.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "mesh-sequence/kick.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/rigid/screw.json >> ../matrixChangeResults/matrixChange_screw.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "rigid/screw.json done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/static/two-cubes.json >> ../matrixChangeResults/matrixChange_two-cubes.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "static/two-cubes.json done"
