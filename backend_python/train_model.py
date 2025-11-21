#!/usr/bin/env python3
"""
ML Model Training Script for Algorithmic Trading - REAL DATA VERSION
"""

import numpy as np
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report, accuracy_score
import joblib
import warnings
import os
warnings.filterwarnings('ignore')


def load_real_data(csv_path='../algo_trading/data/sample_AAPL.csv'):
    """
    Load real stock data from CSV file.
    
    Expected CSV format:
    timestamp,symbol,open,high,low,close,adj_close,volume,bid,ask
    """
    print(f"[INFO] Loading data from: {csv_path}")
    
    if not os.path.exists(csv_path):
        print(f"[ERROR] File not found: {csv_path}")
        print("[INFO] Please ensure sample_AAPL.csv exists in algo_trading/data/")
        return None
    
    # Read CSV
    df = pd.read_csv(csv_path)
    
    print(f"[INFO] Loaded {len(df)} rows")
    print(f"[INFO] Columns: {list(df.columns)}")
    
    # Sort by timestamp
    df = df.sort_values('timestamp').reset_index(drop=True)
    
    return df


def calculate_features(df):
    """
    Calculate technical indicators and features from raw OHLCV data.
    """
    print("\n[INFO] Calculating features...")
    
    # Price returns
    df['return_1'] = df['close'].pct_change(1)  # 1-day return
    df['return_5'] = df['close'].pct_change(5)  # 5-day return
    
    # Moving averages
    df['ma_short'] = df['close'].rolling(window=10, min_periods=1).mean()
    df['ma_long'] = df['close'].rolling(window=50, min_periods=1).mean()
    
    # Volatility (20-day rolling std)
    df['volatility'] = df['close'].rolling(window=20, min_periods=1).std()
    
    # Volume ratio (current volume / 20-day average volume)
    avg_volume = df['volume'].rolling(window=20, min_periods=1).mean()
    df['volume_ratio'] = df['volume'] / avg_volume
    df['volume_ratio'] = df['volume_ratio'].fillna(1.0)
    
    # Price (current close)
    df['price'] = df['close']
    
    # Momentum (5-day price change rate)
    df['momentum'] = (df['close'] - df['close'].shift(5)) / df['close'].shift(5)
    
    # Fill NaN values
    df = df.fillna(method='bfill').fillna(method='ffill')
    
    print(f"[INFO] Features calculated for {len(df)} samples")
    
    return df


def create_labels(df, forward_days=5, threshold=0.01):
    """
    Create BUY/SELL labels based on future returns.
    
    Label = 1 (BUY) if price increases by threshold% in next forward_days
    Label = 0 (SELL) otherwise
    """
    print(f"\n[INFO] Creating labels (forward_days={forward_days}, threshold={threshold*100}%)")
    
    # Calculate future return
    df['future_return'] = df['close'].shift(-forward_days) / df['close'] - 1
    
    # Create labels
    df['label'] = 0  # Default: SELL
    df.loc[df['future_return'] > threshold, 'label'] = 1  # BUY if return > threshold
    
    # Drop rows with NaN labels (last forward_days rows)
    df = df.dropna(subset=['label'])
    
    print(f"[INFO] Labels created for {len(df)} samples")
    print(f"\nClass distribution:")
    print(df['label'].value_counts())
    print(f"\nClass percentages:")
    print(df['label'].value_counts(normalize=True) * 100)
    
    return df


def prepare_features(df):
    """Extract feature columns and labels."""
    feature_cols = [
        'return_1', 'return_5', 'ma_short', 'ma_long',
        'volatility', 'volume_ratio', 'price', 'momentum'
    ]
    
    # Check if all features exist
    missing_cols = [col for col in feature_cols if col not in df.columns]
    if missing_cols:
        print(f"[ERROR] Missing columns: {missing_cols}")
        return None, None
    
    X = df[feature_cols].values
    y = df['label'].values
    
    return X, y


def train_model(X_train, y_train):
    """Train Random Forest classifier."""
    print("\n[INFO] Training Random Forest Classifier...")
    print(f"Training samples: {len(X_train)}")
    
    model = RandomForestClassifier(
        n_estimators=100,
        max_depth=10,
        min_samples_split=20,
        min_samples_leaf=10,
        random_state=42,
        n_jobs=-1
    )
    
    model.fit(X_train, y_train)
    print("[INFO] Training complete!")
    
    return model


def evaluate_model(model, X_test, y_test):
    """Evaluate model performance."""
    print("\n[INFO] Evaluating model...")
    
    y_pred = model.predict(X_test)
    accuracy = accuracy_score(y_test, y_pred)
    
    print(f"\nAccuracy: {accuracy:.4f}")
    print("\nClassification Report:")
    print(classification_report(y_test, y_pred, target_names=['SELL (0)', 'BUY (1)'], zero_division=0))
    
    feature_names = [
        'return_1', 'return_5', 'ma_short', 'ma_long',
        'volatility', 'volume_ratio', 'price', 'momentum'
    ]
    
    importance = model.feature_importances_
    feature_importance = sorted(zip(feature_names, importance), key=lambda x: x[1], reverse=True)
    
    print("\nFeature Importance:")
    for feat, imp in feature_importance:
        print(f"  {feat:20s}: {imp:.4f}")


def save_model(model, filename='model.pkl'):
    """Save trained model to disk."""
    joblib.dump(model, filename)
    print(f"\n[INFO] Model saved to: {filename}")
    print(f"[INFO] File size: {os.path.getsize(filename) / 1024:.2f} KB")


def main():
    """Main training pipeline - REAL DATA VERSION."""
    
    print("=" * 60)
    print("ML MODEL TRAINING - REAL DATA VERSION")
    print("=" * 60)
    
    # Step 1: Load real data
    print("\n[1/6] Loading real stock data...")
    df = load_real_data('../algo_trading/data/sample_AAPL.csv')
    
    if df is None:
        print("[ERROR] Failed to load data. Exiting.")
        return
    
    # Step 2: Calculate features
    print("\n[2/6] Calculating technical features...")
    df = calculate_features(df)
    
    # Step 3: Create labels
    print("\n[3/6] Creating trading labels...")
    df = create_labels(df, forward_days=5, threshold=0.01)
    
    if len(df) < 100:
        print(f"[ERROR] Not enough data samples ({len(df)}). Need at least 100.")
        print("[INFO] Please add more data to sample_AAPL.csv")
        return
    
    # Step 4: Prepare features
    print("\n[4/6] Preparing features...")
    X, y = prepare_features(df)
    
    if X is None:
        print("[ERROR] Feature preparation failed. Exiting.")
        return
    
    print(f"Feature shape: {X.shape}")
    print(f"Label shape: {y.shape}")
    
    # Step 5: Split data
    print("\n[5/6] Splitting train/test sets...")
    
    # Check if we have enough samples for stratification
    if len(df) < 50:
        stratify = None
        print("[WARNING] Too few samples for stratified split. Using regular split.")
    else:
        stratify = y
    
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42, stratify=stratify
    )
    print(f"Train set: {len(X_train)} samples")
    print(f"Test set: {len(X_test)} samples")
    
    # Step 6: Train model
    print("\n[6/6] Training model...")
    model = train_model(X_train, y_train)
    
    # Evaluate
    evaluate_model(model, X_test, y_test)
    
    # Save model
    save_model(model, 'model.pkl')
    
    print("\n" + "=" * 60)
    print("TRAINING COMPLETE!")
    print("=" * 60)
    print("\nYour model was trained on REAL data from sample_AAPL.csv")
    print("\nNext steps:")
    print("1. Start the FastAPI server: python main_ml_api.py")
    print("2. Run the C++ backtester")
    print("\n")


if __name__ == '__main__':
    main()
