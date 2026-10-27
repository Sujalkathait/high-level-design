# System Design: Social Media News Feed (Twitter / Instagram)

> High-Level System Design | Real-World Architecture Case Study

---

### 1. Requirements & Scope
* **Functional:**
  * Users can publish posts (text, images).
  * Users can follow other users.
  * Users can view a reverse-chronological or ranked news feed of posts from people they follow.
* **Non-Functional:**
  * Feed generation latency < 200ms.
  * Eventual consistency for feed updates (5-10s delay acceptable).
  * High read volume (99% reads, 1% writes; e.g. 500k feed reads/sec vs. 5k posts/sec).

---

### 2. High-Level Architecture

```mermaid
flowchart TD
    User[Client Device] --> LB[Load Balancer]
    LB --> PostSvc[Post Publishing Service]
    LB --> FeedSvc[Feed Read Service]
    
    PostSvc --> PostDB[(Post Storage DB)]
    PostSvc --> FanoutQueue[Kafka Fanout Queue]
    
    FanoutQueue --> FanoutWorker[Fanout Workers]
    FanoutWorker --> FollowerDB[(Graph / Social Graph DB)]
    FanoutWorker --> FeedCache[(Redis Feed Timeline Cache)]
    
    FeedSvc <--> FeedCache
```

---

### 3. Key Architectural Decisions: Fan-Out on Write vs. Fan-Out on Read

| Approach | How it Works | Pros | Cons |
|---|---|---|---|
| **Fan-Out on Write (Push)** | When User A posts, immediately push `post_id` into all followers' Redis timeline lists | Reading the feed is an instant $O(1)$ memory lookup | **Celebrity Problem:** If an account with 50M followers posts, 50 million Redis writes are triggered! |
| **Fan-Out on Read (Pull)** | Timeline is only assembled when the follower opens the app | No write amplification | Slower feed read latency ($O(N)$ query across all followees) |
| **Hybrid (Production Standard)** | Push for standard users (< 20k followers); Pull for celebrities (> 20k followers) | Fast feed generation without celebrity write storms | Slightly more complex feed aggregation code |
