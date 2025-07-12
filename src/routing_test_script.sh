#!/bin/bash

echo "=== Single Packet Hop Count Test (5↔10) ==="
echo ""

# First, let's verify your current traffic.cpp settings
echo "📋 Make sure your traffic.cpp has:"
echo "• single_packet: source = 5, dest = 10"
echo "• single_packet_reverse: source = 10, dest = 5"
echo ""

# Test with more samples to get hop count statistics
echo "🧪 Test 1: Forward direction (5→10) with more samples"
cat > hop_5_to_10.cfg << EOF
topology = torus_credit;
k = 4;
n = 2;
routing_function = dim_order;
traffic = single_packet;
injection_rate = 0.1;
sim_count = 200;
sample_period = 1000;
max_samples = 10;
warmup_periods = 2;
single_packet_source = 5;
single_packet_dest = 10;
EOF

echo "Forward (5→10) results:"
timeout 30s ./booksim hop_5_to_10.cfg 2>/dev/null | grep -E "(Hops average|latency average)" | tail -4

echo ""
echo "🧪 Test 2: Reverse direction (10→5) with more samples"
cat > hop_10_to_5.cfg << EOF
topology = torus_credit;
k = 4;
n = 2;
routing_function = dim_order;
traffic = single_packet;
injection_rate = 0.1;
sim_count = 200;
sample_period = 1000;
max_samples = 10;
warmup_periods = 2;
single_packet_source = 10;
single_packet_dest = 5;
EOF

echo "Reverse (10→5) results:"
timeout 30s ./booksim hop_10_to_5.cfg 2>/dev/null | grep -E "(Hops average|latency average)" | tail -4

echo ""
echo "🧪 Test 3: Compare with bidirectional for same nodes"
cat > hop_bidir_5_10.cfg << EOF
topology = torus;
k = 4;
n = 2;
routing_function = dim_order;
traffic = single_packet;
injection_rate = 0.1;
sim_count = 200;
EOF

echo "Bidirectional (5→10) results:"
timeout 30s ./booksim hop_bidir_5_10.cfg 2>/dev/null | grep -E "(Hops average|latency average)" | tail -4

echo ""
echo "📊 Expected for 5(1,1) → 10(2,2):"
echo "• Bidirectional: ~2 hops (direct path: +1X, +1Y)"
echo "• Unidirectional efficient direction: ~2 hops"  
echo "• Unidirectional forced direction: ~6 hops (wrap around)"
echo ""

rm -f hop_5_to_10.cfg hop_10_to_5.cfg hop_bidir_5_10.cfg