#!/usr/bin/env python3
"""
Analyze plugin measurement results from CSV files.
"""

import pandas as pd
import numpy as np
import sys
from pathlib import Path

def analyze_rms_peak(csv_path):
    """Analyze RMS and Peak measurements."""
    print("\n" + "="*80)
    print("RMS & PEAK ANALYSIS")
    print("="*80)

    df = pd.read_csv(csv_path)

    print(f"\nTotal runs: {len(df)}")
    print(f"\nColumns: {list(df.columns)}")

    # Parameter columns (excluding measurement columns)
    param_cols = [c for c in df.columns if c not in
                  ['runId', 'inputGainDb', 'rmsInL', 'rmsInR', 'rmsOutL', 'rmsOutR',
                   'peakInL', 'peakInR', 'peakOutL', 'peakOutR']]

    print(f"\nParameter columns: {param_cols}")

    # Statistics by input gain
    print("\n--- Statistics by Input Gain ---")
    for gain in sorted(df['inputGainDb'].unique()):
        gain_data = df[df['inputGainDb'] == gain]
        print(f"\nInput Gain: {gain} dB")
        print(f"  RMS Out L: mean={gain_data['rmsOutL'].mean():.6f}, "
              f"std={gain_data['rmsOutL'].std():.6f}, "
              f"min={gain_data['rmsOutL'].min():.6f}, "
              f"max={gain_data['rmsOutL'].max():.6f}")
        print(f"  Peak Out L: mean={gain_data['peakOutL'].mean():.6f}, "
              f"std={gain_data['peakOutL'].std():.6f}, "
              f"min={gain_data['peakOutL'].min():.6f}, "
              f"max={gain_data['peakOutL'].max():.6f}")

        # Calculate gain (output/input ratio)
        gain_ratio = gain_data['rmsOutL'] / gain_data['rmsInL']
        print(f"  Gain Ratio (Out/In): mean={gain_ratio.mean():.6f}, "
              f"std={gain_ratio.std():.6f}")

    # Check for clipping (peak >= 1.0)
    clipping = df[df['peakOutL'] >= 1.0]
    if len(clipping) > 0:
        print(f"\n⚠️  CLIPPING DETECTED: {len(clipping)} runs with peak >= 1.0")
        print(f"   Input gains with clipping: {sorted(clipping['inputGainDb'].unique())}")
    else:
        print("\n✅ No clipping detected (all peaks < 1.0)")

    return df

def analyze_thd(csv_path):
    """Analyze Total Harmonic Distortion measurements."""
    print("\n" + "="*80)
    print("THD (Total Harmonic Distortion) ANALYSIS")
    print("="*80)

    df = pd.read_csv(csv_path)

    print(f"\nTotal measurements: {len(df)}")
    print(f"Unique runs: {df['runId'].nunique()}")
    print(f"Measurements per run: ~{len(df) / df['runId'].nunique():.0f}")

    # Statistics by input gain
    print("\n--- THD Statistics by Input Gain ---")
    for gain in sorted(df['inputGainDb'].unique()):
        gain_data = df[df['inputGainDb'] == gain]
        thd_db = 20 * np.log10(gain_data['thd'] + 1e-10)  # Convert to dB
        print(f"\nInput Gain: {gain} dB")
        print(f"  THD (linear): mean={gain_data['thd'].mean():.2e}, "
              f"std={gain_data['thd'].std():.2e}, "
              f"min={gain_data['thd'].min():.2e}, "
              f"max={gain_data['thd'].max():.2e}")
        print(f"  THD (dB): mean={thd_db.mean():.2f} dB, "
              f"std={thd_db.std():.2f} dB, "
              f"min={thd_db.min():.2f} dB, "
              f"max={thd_db.max():.2f} dB")

    # Find runs with highest THD
    print("\n--- Highest THD Runs (Top 10) ---")
    top_thd = df.nlargest(10, 'thd')[['runId', 'thd', 'inputGainDb'] +
                                      [c for c in df.columns if c not in
                                       ['runId', 'centreSample', 'thd', 'inputGainDb']]]
    for idx, row in top_thd.iterrows():
        thd_db = 20 * np.log10(row['thd'] + 1e-10)
        params = {c: row[c] for c in top_thd.columns if c not in ['runId', 'thd', 'inputGainDb']}
        print(f"  Run {int(row['runId'])}: THD={row['thd']:.2e} ({thd_db:.2f} dB), "
              f"Gain={row['inputGainDb']} dB, Params={params}")

    return df

def analyze_transfer_curves(csv_path):
    """Analyze transfer curve measurements."""
    print("\n" + "="*80)
    print("TRANSFER CURVE ANALYSIS")
    print("="*80)

    df = pd.read_csv(csv_path)

    print(f"\nTotal measurements: {len(df)}")
    print(f"Unique runs: {df['runId'].nunique()}")
    print(f"Bins per run: ~{len(df) / df['runId'].nunique():.0f}")

    # Statistics by input gain
    print("\n--- Transfer Curve Statistics by Input Gain ---")
    for gain in sorted(df['inputGainDb'].unique()):
        gain_data = df[df['inputGainDb'] == gain]
        print(f"\nInput Gain: {gain} dB")
        print(f"  Input range (x): [{gain_data['x'].min():.6f}, {gain_data['x'].max():.6f}]")
        print(f"  Output range (meanY): [{gain_data['meanY'].min():.6f}, {gain_data['meanY'].max():.6f}]")
        print(f"  Linear region check:")

        # Check linearity in small signal region
        small_signal = gain_data[(gain_data['x'].abs() < 0.1)]
        if len(small_signal) > 0:
            # Fit linear model: y = a*x
            x_vals = small_signal['x'].values
            y_vals = small_signal['meanY'].values
            gain_factor = np.polyfit(x_vals, y_vals, 1)[0]
            print(f"    Small signal gain (|x|<0.1): {gain_factor:.6f}")

        # Check for saturation
        max_output = gain_data['meanY'].abs().max()
        if max_output > 0.95:
            print(f"    ⚠️  Saturation detected: max output = {max_output:.6f}")
        else:
            print(f"    ✅ No saturation: max output = {max_output:.6f}")

    # Analyze linearity
    print("\n--- Linearity Analysis ---")
    for gain in sorted(df['inputGainDb'].unique()):
        gain_data = df[df['inputGainDb'] == gain].copy()
        gain_data = gain_data.sort_values('x')

        # Calculate deviation from linear
        if len(gain_data) > 1:
            # Simple linear fit
            x_vals = gain_data['x'].values
            y_vals = gain_data['meanY'].values
            coeffs = np.polyfit(x_vals, y_vals, 1)
            y_pred = np.polyval(coeffs, x_vals)
            residuals = y_vals - y_pred
            rmse = np.sqrt(np.mean(residuals**2))

            print(f"\nInput Gain {gain} dB:")
            print(f"  Linear fit: y = {coeffs[0]:.6f}*x + {coeffs[1]:.6f}")
            print(f"  RMSE from linear: {rmse:.6f}")
            print(f"  Max deviation: {np.abs(residuals).max():.6f}")

    return df

def main():
    output_dir = Path("/Users/lucaromagnoli/plugin_measure_output")

    if not output_dir.exists():
        print(f"Error: Output directory not found: {output_dir}")
        sys.exit(1)

    print("="*80)
    print("PLUGIN MEASUREMENT RESULTS ANALYSIS")
    print("="*80)
    print(f"\nAnalyzing results in: {output_dir}")

    # Analyze each CSV file
    rms_peak_path = output_dir / "grid_rms_peak.csv"
    thd_path = output_dir / "grid_thd.csv"
    transfer_path = output_dir / "grid_transfer_curves.csv"
    raw_path = output_dir / "raw.csv"

    if rms_peak_path.exists():
        rms_df = analyze_rms_peak(rms_peak_path)
    else:
        print(f"\n⚠️  {rms_peak_path} not found")
        rms_df = None

    if thd_path.exists():
        thd_df = analyze_thd(thd_path)
    else:
        print(f"\n⚠️  {thd_path} not found")
        thd_df = None

    if transfer_path.exists():
        transfer_df = analyze_transfer_curves(transfer_path)
    else:
        print(f"\n⚠️  {transfer_path} not found")
        transfer_df = None

    if raw_path.exists():
        print("\n" + "="*80)
        print("RAW DATA FILE")
        print("="*80)
        print(f"\n⚠️  Raw CSV file is very large: {raw_path.stat().st_size / (1024**3):.2f} GB")
        print("   Skipping detailed analysis. Use for sample-by-sample inspection if needed.")

    print("\n" + "="*80)
    print("ANALYSIS COMPLETE")
    print("="*80)

if __name__ == "__main__":
    main()
