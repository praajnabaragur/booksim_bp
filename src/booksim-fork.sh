#!/bin/zsh

# Booksim fork script for unidirectional torus topology
# Based on Q4 conditions but using torus_credit topology

BOOKSIM=./booksim  # Using local build
BASE_CFG=base_torus_credit.cfg
OUT_CSV=results.csv

echo "Size,VCs,Traffic,InjectionRate,AvgLatency,MaxLatency" > $OUT_CSV

ks=(2 4 8)
vcs=(1 2 3)
injection_rates=(0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0)
traffic_patterns=(uniform transpose)

mkdir -p logs # Ensure logs directory exists

for k in $ks; do
  for vc in $vcs; do
    for inj in $injection_rates; do
      for traffic in $traffic_patterns; do
        CONFIG=temp_${k}x${k}_vc${vc}_${traffic}_inj${inj}.cfg
        cp "$BASE_CFG" "$CONFIG"

        # Append the specific varying parameters with semicolons
        echo "k = $k;" >> "$CONFIG"
        echo "num_vcs = $vc;" >> "$CONFIG"
        echo "n = 2;" >> "$CONFIG"
        echo "traffic = $traffic;" >> "$CONFIG"
        echo "injection_rate = $inj;" >> "$CONFIG"
        echo "routing_function = dim_order;" >> "$CONFIG"

        echo "Running simulation for Size: ${k}x${k}, VCs: $vc, Traffic: $traffic, Injection Rate: $inj (Unidirectional Torus)"
        
        # Run BookSim and capture output, piping to tee to save to log file
        OUT=$("$BOOKSIM" "$CONFIG" | tee "logs/fork_${k}x${k}_vc${vc}_${traffic}_inj${inj}.log")

        # Extract latency values from the "Overall Traffic Statistics" section (last occurrence)
        avg_lat=$(echo "$OUT" | tac | grep -m 1 "Packet latency average =" | awk -F' = ' '{print $2}' | awk '{print $1}')
        max_lat=$(echo "$OUT" | tac | grep -m 1 "maximum =" | awk -F' = ' '{print $2}' | awk '{print $1}')

        # Handle cases where latency might be missing
        if [[ -z "$avg_lat" ]]; then avg_lat="N/A"; fi
        if [[ -z "$max_lat" ]]; then max_lat="N/A"; fi

        echo "${k}x${k},$vc,$traffic,$inj,$avg_lat,$max_lat" >> "$OUT_CSV"
        
        # Clean up temporary config
        rm "$CONFIG"
      done
    done
  done
done

echo "Unidirectional torus simulation sweep completed. Results saved to $OUT_CSV"