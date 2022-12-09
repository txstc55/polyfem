cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/stress-tests/dolphin-funnel.json >> ../matrixChangeResults/matrixChange_dolphin-funnel.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "stress-tests/dolphin-funnel done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/stress-tests/mat-twist.json >> ../matrixChangeResults/matrixChange_mat-twist.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "stress-tests/mat-twist done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/stress-tests/rod-twist.json >> ../matrixChangeResults/matrixChange_rod-twist.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "stress-tests/rod-twist done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/stress-tests/squeeze-out.json >> ../matrixChangeResults/matrixChange_squeeze-out.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "stress-tests/squeeze-out done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/stress-tests/trash-compactor-octocat.json >> ../matrixChangeResults/matrixChange_trash-compactor-octocat.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "stress-tests/trash-compactor-octocat done"
cd build
./PolyFEM_bin --max_threads 8 -j ../data/contact/examples/3D/stress-tests/trash-compactor-shapes.json >> ../matrixChangeResults/matrixChange_trash-compactor-shapes.txt
rm -rf *.vtm
rm -rf *.vtu
cd ..
echo "stress-tests/trash-compactor-shapes done"