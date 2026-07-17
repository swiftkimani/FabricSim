import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import seaborn as sns
import os

# Create a figure and axis
fig, ax = plt.subplots(figsize=(12, 8))

def animate(i):
    if not os.path.exists("telemetry.csv"):
        return

    try:
        # Read the CSV
        df = pd.read_csv("telemetry.csv")
        
        if df.empty:
            return

        # Pivot to create a heatmap grid: rows=node_id, cols=tick, values=queue_depth
        heatmap_data = df.pivot(index='node_id', columns='tick', values='queue_depth')
        
        # Clear the entire figure to prevent colorbar stacking
        fig.clf()
        ax = fig.add_subplot(111)
        
        # Draw the heatmap
        sns.heatmap(heatmap_data, ax=ax, cmap="YlOrRd", cbar_kws={'label': 'Queue Depth (Packets)'})
        
        ax.set_title("Live Network Congestion Heatmap (Queue Depth over Time)")
        ax.set_xlabel("Simulation Tick")
        ax.set_ylabel("Node ID (1-4: Core, 5-12: Agg, 13-20: Edge, 21-36: Host)")
        
    except pd.errors.EmptyDataError:
        pass
    except Exception as e:
        print(f"Error drawing heatmap: {e}")

# Run animation
ani = animation.FuncAnimation(fig, animate, interval=1000, cache_frame_data=False)

plt.tight_layout()
plt.show()
