# **Product Requirements Document: 3D Missile Command - Survivor**

## **Executive Summary**

This document outlines the product requirements for **3D Missile Command: Survivor**, a modern 3D reimagining of the classic arcade game, transformed into a "bullet heaven" with deep meta-progression. Built in Unreal Engine 5, the game challenges players to defend static cities from overwhelming enemy swarms. Success hinges on creating powerful weapon builds via in-world Supply Drops, managing a dual economy of experience and resources, and protecting not only cities but also vulnerable economic units.

## **Game Overview**

### **Core Concept**

Players command a planetary defense network to protect several cities from relentless, descending enemies. The core loop involves destroying enemies to earn experience (XP) for in-run upgrades and raw materials ("Resources") for permanent meta-progression. XP is gathered by automated **Collector** drones to trigger **Supply Drops**, which deliver randomized upgrades without pausing the action. Resources are gathered by fragile, destructible **Scavenger Rovers** that must be actively defended. The session ends when all cities are destroyed.

### **Target Platform**

*   **PC (Windows)**
*   **Engine:** Unreal Engine 5.6

## **Core Gameplay**

### **Primary Objective**

*   **Protect your cities.** The primary failure state is the destruction of all defended cities.
*   **Protect your Scavenger Rovers.** The loss of these units will cripple your meta-progression.
*   Survive for a 20-minute timed session against increasingly overwhelming enemy hordes.

### **Game Flow**

1.  **Session-Based Survival:** Each game session is a timed run on a map with a unique city layout.
2.  **Dual-Resource Economy:**
    *   Defeated enemies drop **XP Gems**. These are automatically retrieved by non-destructible **Collector** units.
    *   Defeated enemies also drop **Resources** (scrap metal, components). These must be retrieved by destructible **Scavenger Rovers**.
3.  **Supply Drop Progression:** As Collectors gather XP, an experience bar fills. When full, a **Supply Drop** is called in. The player can shoot the crate as it parachutes down or click on it after it lands. Choosing an upgrade triggers a brief "bullet time" slow-motion effect, allowing for a tactical choice without stopping the game.
4.  **Meta-Progression:** Resources successfully gathered by Scavenger Rovers are used after the run to purchase permanent global upgrades.

## **Defense & Upgrade Systems**

### **City Infrastructure**

The cities are the player's primary health pool and an active part of the defense.

*   **City Health:** Each city has its own health bar.
*   **City Modules & Synergies:** Upgrades can be applied to cities, turning them into defensive assets.
    *   **Examples:** A **Voltaic Shield** that damages attackers, Repair Systems, or automated Anti-Air Turrets. These can create synergies with the player's main weapon systems.

### **Weapon Systems (Acquired from Supply Drops)**

#### **Player-Controlled Systems**
*   **Type:** Anti-Missile Batteries, Railguns.
*   **Features:** Fired manually by the player's mouse reticle. Rewards precision and target prioritization.

#### **Automated Ordnance Systems**
*   **Type:** Missile Launchers, Mortar Batteries, Flak Cannons, Drone Swarms.
*   **Features:** Once unlocked, these fire automatically based on their own cooldowns and targeting priorities.

### **Utility Systems**

#### **Collector Drones (XP Gathering)**
*   **Purpose:** Automatically seek out and retrieve XP gems from defeated enemies to fill the Supply Drop meter.
*   **Features:** Cannot be damaged or destroyed.
*   **Upgrades:** Increase collection speed, range, or the amount of XP gained per gem.

#### **Scavenger Rovers (Resource Gathering)**
*   **Purpose:** Deploy from a central command building to retrieve Resources (meta-currency) from defeated enemies.
*   **Features:** Are physical, ground-based vehicles that can be destroyed by enemy fire. A key secondary defense objective.
*   **Upgrades (Meta):** Increase Rover health, speed, or the number of active Rovers.

### **Global Passive Buffs (Acquired from Supply Drops)**
*   **Weapon Buffs:** Overcharge Capacitors (crit chance), Projectile Velocity Matrix (projectile speed), Homing Algorithms (slight homing), Ablative Munitions (piercing shots).
*   **Tactical Buffs:** Threat Prediction Matrix (highlights special enemies), Strategic Recalculation (upgrade rerolls), Construction Nanites (automatically rebuilds destroyed Scavenger Rovers after a long cooldown).

### **Commander Abilities**

These are powerful, long-cooldown abilities that differentiate the player as a "commander."

*   **Strategic View:** The player can temporarily zoom out to a wider, tactical battlefield view.
*   **Abilities:** From this view, the player can deploy game-changing abilities such as:
    *   **Designate Priority Target:** Forces all automated weapons to focus fire on a single target for a short duration.
    *   **A-10 Strafing Run:** Calls in a friendly aircraft to perform a devastating strafing run across a designated path.
    *   **Deploy Decoy:** Deploys a temporary beacon that draws enemy fire away from cities.

## **Battlefield Dynamics**

### **Environmental Factors**
*   The battlefield is not static. Events can occur that affect both players and enemies.
*   **Example: Meteor Showers:** A neutral hazard that rains down debris, damaging anything it hits—cities, enemies, and your Scavenger Rovers.

## **Meta-Progression System**

### **Global Upgrades (Command Center Perks)**
*   Players spend **Resources** gathered by Scavenger Rovers to permanently improve their entire defense network between runs.
*   **Upgrades:** Increase starting city health, boost base weapon damage, improve Rover durability, increase starting Collectors, unlock new Commander Abilities.

### **Unlocks**
*   **New Weapon Systems:** Expand the pool of potential upgrades from Supply Drops.
*   **New City Modules:** Unlock new types of defensive structures.
*   **New Maps & City Layouts:** Unlock different environments.

### **Pre-Run Strategy**
*   Before a run begins, players can make strategic choices based on their unlocks.
*   **Example: City Layout Selection:** Choose how your cities are arranged on the map—clustered for tight AoE defense or spread out to mitigate splash damage.

## **Technical Requirements & Risks**

*(This section remains largely the same, focusing on Niagara for VFX, scalability, and prioritizing framerate over features like Lumen. The primary technical risk remains maintaining 60 FPS with a high number of on-screen entities.)*

## **Risk Assessment (Revised)**

### **Design Risks**
*   **Cognitive Load:** The player must now track city health, their own aiming, incoming threats, Supply Drops, AND the safety of their Scavenger Rovers. This complexity must be introduced carefully to avoid overwhelming new players.
*   **Balance:** The dual-economy must be balanced. The risk of losing a Rover must be justified by the reward. Player-controlled weapons must remain engaging alongside powerful automated systems and Commander Abilities.
*   **Market Saturation:** The bullet heaven genre is crowded. The unique combination of active aiming, city defense, destructible units, and commander abilities must be polished to create a distinct identity.
*   **Visual Noise:** With even more units (Rovers, Collectors) and effects on screen, maintaining visual clarity so the player can make informed decisions is a paramount challenge.