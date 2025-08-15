import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
df = pd.read_csv("results_bp.csv")

# Ensure correct types
df['InjectionRate'] = df['InjectionRate'].astype(float)
df['AvgLatency'] = df['AvgLatency'].astype(float)
df['MaxLatency'] = df['MaxLatency'].astype(float)

# Unique network sizes
sizes = df['Size'].unique()

# Create subplots: one row per size, two columns (AvgLatency / MaxLatency)
fig, axes = plt.subplots(len(sizes), 2, figsize=(12, 5 * len(sizes)), sharex=True)

# If only one size, wrap in list for consistency
if len(sizes) == 1:
    axes = [axes]

for i, size in enumerate(sizes):
    subdf = df[df['Size'] == size]
    ax_avg = axes[i][0]
    ax_max = axes[i][1]
    
    # Plot each Traffic / VC combination
    for (traffic, vcs), group in subdf.groupby(['Traffic', 'VCs']):
        ax_avg.plot(group['InjectionRate'], group['AvgLatency'], marker='o', label=f"{traffic}, {vcs} VC")
        ax_max.plot(group['InjectionRate'], group['MaxLatency'], marker='o', label=f"{traffic}, {vcs} VC")
    
    # Titles and labels
    ax_avg.set_title(f"{size} - Average Latency vs Injection Rate")
    ax_avg.set_xlabel("Injection Rate")
    ax_avg.set_ylabel("Average Latency")
    ax_avg.legend()
    ax_avg.grid(True)
    
    ax_max.set_title(f"{size} - Maximum Latency vs Injection Rate")
    ax_max.set_xlabel("Injection Rate")
    ax_max.set_ylabel("Maximum Latency")
    ax_max.legend()
    ax_max.grid(True)

plt.tight_layout()
plt.savefig("bp_results.png", dpi=300)
