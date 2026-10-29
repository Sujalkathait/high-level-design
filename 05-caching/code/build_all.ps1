# Helper script to compile all Caching simulations
Write-Host "Compiling 05-caching C++ programs..." -ForegroundColor Cyan

g++ -std=c++17 lru_cache.cpp -o lru_cache.exe
g++ -std=c++17 lfu_cache.cpp -o lfu_cache.exe
g++ -std=c++17 fifo_cache.cpp -o fifo_cache.exe
g++ -std=c++17 ttl_cache.cpp -o ttl_cache.exe
g++ -std=c++17 cache_aside.cpp -o cache_aside.exe

Write-Host "All 5 Caching programs compiled successfully!" -ForegroundColor Green

