import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

def plot_params(in_file, out_dir, ylabel):
    df = pd.read_csv(in_file)        
    columns = ['Size', 'Type', 'StandardMerge', 'StandardQuick', 
                        'CustomMerge', 'CustomQuick', 'Radix', 'RadixQuick']
    
    sortings = columns[2:]
    
    os.makedirs(out_dir, exist_ok=True)
    
    for dtype in df['Type'].unique():
        plt.figure(figsize=(12, 8))
        sns.set_style("whitegrid")
        
        subset = df[df['Type'] == dtype]
        
        for sorting in sortings:
            plt.plot(subset['Size'], subset[sorting], 
                    label=sorting, linewidth=2)
        
        plt.xlabel('Input Size', fontsize=12)
        plt.ylabel(ylabel, fontsize=12)
        plt.title(f'Performance ({dtype} Dataset)', fontsize=14)
        plt.legend(fontsize=10, bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.tight_layout()
        
        filename = f"{ylabel.split('(')[0].strip().lower()}_{dtype.lower()}.png"
        plt.savefig(f'{out_dir}/{filename}', dpi=300, bbox_inches='tight')
        plt.close()

def main():
    out_file = 'performance_plots'
    os.makedirs('performance_plots', exist_ok=True)
    
    if os.path.exists('microseconds_results.csv'):
        print("Processing time measurements...")
        plot_params('microseconds_results.csv', 
                    f'{out_file}/time', 
                    'Execution Time (мс)')
    
    if os.path.exists('comparisons_results.csv'):
        print("Processing comparison counts...")
        plot_params('comparisons_results.csv', 
                    f'{out_file}/comparisons', 
                    'Comparison Count')
    
    print(f"\nAll plots saved to '{out_file}' directory")
        

if __name__ == "__main__":
    main()