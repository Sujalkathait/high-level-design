# Helper script to compile all Load Balancing simulations
Write-Host "Compiling 04-load-balancing C++ programs..." -ForegroundColor Cyan

g++ -std=c++17 round_robin.cpp -o round_robin.exe
g++ -std=c++17 weighted_round_robin.cpp -o weighted_round_robin.exe
g++ -std=c++17 least_connections.cpp -o least_connections.exe
g++ -std=c++17 ip_hash.cpp -o ip_hash.exe
g++ -std=c++17 consistent_hashing.cpp -o consistent_hashing.exe
g++ -std=c++17 health_check.cpp -o health_check.exe

Write-Host "All 6 Load Balancing programs compiled successfully!" -ForegroundColor Green
