# Open World Framework

### 🔊 Physical Audio & Surface Detection
To enhance immersion, I implemented a systemic surface detection framework:
* **Physics-Driven:** The system queries the `Physical Material` of world geometry rather than using string-based tags.
* **Data-Driven:** All SFX are managed via a `PrimaryDataAsset`, allowing designers to map new surfaces (Grass, Metal, Snow) without touching C++.
* **Optimized Execution:** Using custom `AnimNotifies`, line traces only execute at the exact moment of foot impact, minimizing CPU overhead compared to per-frame polling.
