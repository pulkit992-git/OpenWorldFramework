# Open World Framework

### 🔊 Physical Audio & Surface Detection
To enhance immersion, I implemented a systemic surface detection framework:
* **Physics-Driven:** The system queries the `Physical Material` of world geometry rather than using string-based tags.
* **Data-Driven:** All SFX are managed via a `PrimaryDataAsset`, allowing designers to map new surfaces (Grass, Metal, Snow) without touching C++.
* **Optimized Execution:** Using custom `AnimNotifies`, line traces only execute at the exact moment of foot impact, minimizing CPU overhead compared to per-frame polling.

---

## 🧲 Physics-Based Telekinesis (Grab & Launch)
Developed a robust interaction system that allows players to manipulate world objects using real-time physics.

* **Physics Handle Integration:** Utilizes `UPhysicsHandleComponent` to create a "soft" tether between the player and objects, allowing for realistic collisions and weight response while carrying items.
* **Predictive Targeting:** Implements a localized targeting system that keeps grabbed objects centered in front of the player's torso while maintaining a procedural "floating" idle state via Sine-wave oscillation.
* **Enhanced Input System:** Fully integrated with UE5's Enhanced Input, supporting distinct states for `Grab`, `Hold` (looping audio/VFX), and `Launch` (impulse-based throwing).

---

## 🦾 Procedural Interaction Animation
Rather than using rigid montages, I implemented a procedural arm-blending system to maintain player mobility during interaction.

* **State-Driven Locomotion Caching:** Utilizes `New Saved Cached Pose` to snapshot core locomotion, ensuring fluid movement regardless of upper-body state.
* **Non-Destructive Blending:** Employs `Layered Blend Per Bone` with a branch filter on `upperarm_r`, isolating the interaction arm without interrupting skeletal locomotion.
* **Temporal Interpolation:** Integrated `Blend Poses by bool` with asymmetrical blend times (0.4s Rise / 0.3s Fall) for a natural, kinetic feel during the grab/release cycle.

---

## ✨ Multi-Layered Feedback & FX
* **Dynamic Audio Management:**
    * **Looping Energy:** Implemented an `UAudioComponent` reference system to start a "telekinesis hum" upon grabbing.
    * **Automated Cleanup:** Logic ensures the audio component triggers a `Stop` immediately upon object release or launch.
* **Impact Visuals:** Spawns Niagara "Muzzle Flash" particles at the point of release during high-velocity launches.

---

### Technical Specifications
* **Engine:** Unreal Engine 5.6
* **Language:** C++ / Blueprints
* **Input:** Enhanced Input System
* **FX:** Niagara Visual Effects