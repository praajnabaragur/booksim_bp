import pandas as pd
import matplotlib.pyplot as plt

# Load CSVs
df_bp = pd.read_csv("results_bp.csv")
df_credit = pd.read_csv("results.csv")

# Filter credit results to only 1 VC
df_credit = df_credit[df_credit['VCs'] == 1]

# Ensure correct types for both
for df in [df_bp, df_credit]:
    df['InjectionRate'] = df['InjectionRate'].astype(float)
    df['AvgLatency'] = df['AvgLatency'].astype(float)
    df['MaxLatency'] = df['MaxLatency'].astype(float)

# Unique network sizes
sizes = sorted(df_bp['Size'].unique())

# Create subplots: one row per size, two columns (AvgLatency / MaxLatency)
fig, axes = plt.subplots(len(sizes), 2, figsize=(12, 5 * len(sizes)), sharex=True)

# Handle case where sizes has only one entry
if len(sizes) == 1:
    axes = [axes]

for i, size in enumerate(sizes):
    ax_avg = axes[i][0]
    ax_max = axes[i][1]

    # Filter for this size
    sub_bp = df_bp[df_bp['Size'] == size]
    sub_credit = df_credit[df_credit['Size'] == size]

    # Plot BP results
    for (traffic, vcs), group in sub_bp.groupby(['Traffic', 'VCs']):
        ax_avg.plot(group['InjectionRate'], group['AvgLatency'], marker='o',
                    label=f"{traffic}, {vcs} VC (BP)")
        ax_max.plot(group['InjectionRate'], group['MaxLatency'], marker='o',
                    label=f"{traffic}, {vcs} VC (BP)")

    # Plot Credit results (only 1 VC now)
    for (traffic, vcs), group in sub_credit.groupby(['Traffic', 'VCs']):
        ax_avg.plot(group['InjectionRate'], group['AvgLatency'], marker='x', linestyle='--',
                    label=f"{traffic}, {vcs} VC (Credit)")
        ax_max.plot(group['InjectionRate'], group['MaxLatency'], marker='x', linestyle='--',
                    label=f"{traffic}, {vcs} VC (Credit)")

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
plt.savefig("bp_vs_credit_results.png", dpi=300)
