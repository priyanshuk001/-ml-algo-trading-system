#!/usr/bin/env python3
"""
FastAPI ML Prediction Server for Algorithmic Trading
"""

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field
from typing import List
import joblib
import numpy as np
import uvicorn
import logging
from datetime import datetime
import os

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

app = FastAPI(
    title="ML Trading Prediction API",
    description="Machine Learning prediction service for algorithmic trading",
    version="1.0.0"
)

model = None
MODEL_FILE = "model.pkl"

class PredictionRequest(BaseModel):
    """Request schema for prediction endpoint."""
    symbol: str = Field(..., description="Stock ticker symbol")
    timestamp: int = Field(..., description="Unix timestamp")
    features: List[float] = Field(
        ..., 
        description="Feature vector [return_1, return_5, ma_short, ma_long, volatility, volume_ratio, price, momentum]",
        min_items=8,
        max_items=8
    )

class PredictionResponse(BaseModel):
    """Response schema for prediction endpoint."""
    symbol: str
    timestamp: int
    prediction: int = Field(..., description="0 = SELL, 1 = BUY")
    probabilities: List[float] = Field(..., description="[P(SELL), P(BUY)]")
    score: float = Field(..., description="Confidence score")
    model_version: str

class HealthResponse(BaseModel):
    """Health check response."""
    status: str
    model_loaded: bool
    model_version: str
    timestamp: str

@app.on_event("startup")
async def load_model():
    """Load the trained model when server starts."""
    global model
    
    logger.info("=" * 60)
    logger.info("STARTING ML PREDICTION SERVER")
    logger.info("=" * 60)
    
    try:
        if not os.path.exists(MODEL_FILE):
            logger.error(f"Model file not found: {MODEL_FILE}")
            logger.error("Please run 'python train_model.py' first!")
            raise FileNotFoundError(f"Model file {MODEL_FILE} not found")
        
        logger.info(f"Loading model from: {MODEL_FILE}")
        model = joblib.load(MODEL_FILE)
        logger.info("✓ Model loaded successfully!")
        logger.info(f"Model type: {type(model).__name__}")
        logger.info("=" * 60)
        
    except Exception as e:
        logger.error(f"Failed to load model: {str(e)}")
        raise

@app.get("/")
async def root():
    """Root endpoint."""
    return {
        "message": "ML Trading Prediction API",
        "version": "1.0.0",
        "docs": "/docs"
    }

@app.get("/health", response_model=HealthResponse)
async def health_check():
    """Health check endpoint."""
    return HealthResponse(
        status="healthy" if model is not None else "model_not_loaded",
        model_loaded=model is not None,
        model_version=MODEL_FILE if model is not None else "none",
        timestamp=datetime.utcnow().isoformat()
    )

@app.post("/predict", response_model=PredictionResponse)
async def predict(request: PredictionRequest):
    """Make trading prediction."""
    
    if model is None:
        raise HTTPException(
            status_code=503, 
            detail="Model not loaded"
        )
    
    try:
        logger.info(f"Prediction request for {request.symbol} @ {request.timestamp}")
        
        features_array = np.array(request.features).reshape(1, -1)
        
        if features_array.shape[1] != 8:
            raise ValueError(f"Expected 8 features, got {features_array.shape[1]}")
        
        prediction = int(model.predict(features_array)[0])
        probabilities = model.predict_proba(features_array)[0].tolist()
        score = probabilities[prediction]
        
        response = PredictionResponse(
            symbol=request.symbol,
            timestamp=request.timestamp,
            prediction=prediction,
            probabilities=probabilities,
            score=float(score),
            model_version=MODEL_FILE
        )
        
        logger.info(f"Prediction: {prediction} (score: {score:.4f})")
        
        return response
        
    except ValueError as e:
        logger.error(f"Validation error: {str(e)}")
        raise HTTPException(status_code=400, detail=str(e))
    except Exception as e:
        logger.error(f"Prediction error: {str(e)}")
        raise HTTPException(status_code=500, detail=str(e))

def main():
    """Run the FastAPI server."""
    uvicorn.run(
        "main_ml_api:app",
        host="0.0.0.0",
        port=8000,
        reload=False,
        log_level="info"
    )

if __name__ == "__main__":
    main()
