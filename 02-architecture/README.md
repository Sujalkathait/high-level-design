# 02. System Architecture

[🏠 Home](../README.md) > **02. Architecture**

---

## Overview

System Architecture defines how software components are organized, bounded, and deployed. Choosing the right architectural paradigm balances development speed, team coordination, system resilience, and runtime performance.

```mermaid
flowchart LR
    A[01. Monolithic Architecture] --> B[02. N-Tier Layering]
    B --> C[03. Microservices Decomposition]
```

---

## Module Table of Contents

| # | Topic | Key Focus | Link |
|---|---|---|---|
| **01** | **Monolithic Architecture** | Single deployable unit, module composition, when to use, modular monolith (Shopify) | [Read Guide ➡️](./monolithic.md) |
| **02** | **N-Tier Architecture** | Presentation, application, and data layers; separation of concerns; monolith vs. N-tier | [Read Guide ➡️](./n-tier.md) |
| **03** | **Microservices Architecture** | Autonomous services, database per service, organizational scaling, Uber case study | [Read Guide ➡️](./microservices.md) |

---

| ⬅️ Previous Module | 🏠 Index | Next Module ➡️ |
|:--- |:---:| ---:|
| [01. Fundamentals](../01-fundamentals/README.md) | [System Design Hub](../README.md) | [03. Networking & Communication ➡️](../03-networking/README.md) |
