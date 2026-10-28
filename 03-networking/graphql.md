# 03. GraphQL

[🏠 Home](../README.md) > [03. Networking](./README.md) > **GraphQL**

---

## 1. What is GraphQL?

**GraphQL** is an open-source query language and server-side execution runtime created by Meta. It allows clients to define the **exact shape and fields of data they need**, retrieving complex relational graphs in a **single network request** over a single endpoint (typically `POST /graphql`).

```mermaid
flowchart LR
    subgraph ClientQuery [Client Request]
        Q["query {\n  user(id: 12) {\n    name\n    orders {\n      id\n      total\n    }\n  }\n}"]
    end

    subgraph GraphQLServer [GraphQL Gateway Engine]
        Schema[Typed Schema & Resolvers]
    end

    subgraph Backends [Microservices / DB]
        UserDB[(User Service)]
        OrderDB[(Order Service)]
    end

    ClientQuery -->|POST /graphql| GraphQLServer
    GraphQLServer -->|Resolve User| UserDB
    GraphQLServer -->|Resolve Orders| OrderDB
    GraphQLServer -->>|Tailored JSON Response| ClientQuery
```

---

## 2. Example Query and Response

### Client Query
```graphql
query GetUserProfileWithOrders {
  user(id: 12) {
    name
    email
    orders {
      id
      total
    }
  }
}
```

### Server JSON Response
```json
{
  "data": {
    "user": {
      "name": "Asha Verma",
      "email": "asha@example.com",
      "orders": [
        { "id": 101, "total": 499 },
        { "id": 102, "total": 1299 }
      ]
    }
  }
}
```

Notice that the structure of the JSON response mirrors the requested query fields line-for-line.

---

## 3. The Three GraphQL Operation Types

1. **Query (Read):** Fetches data without side effects (equivalent to `GET` in REST).
2. **Mutation (Write):** Creates, updates, or deletes data (equivalent to `POST`, `PUT`, `DELETE` in REST).
3. **Subscription (Real-Time):** Establishes an event stream (usually over WebSockets) where the server pushes real-time data to clients whenever a specific event occurs.

---

## 4. The Classic GraphQL Gotcha: The N+1 Query Problem

In naive GraphQL resolver implementations:
1. The server executes **1 SQL query** to fetch 50 users:
   ```sql
   SELECT * FROM users LIMIT 50;
   ```
2. For each user, the nested `orders` field resolver executes a separate query:
   ```sql
   SELECT * FROM orders WHERE user_id = ?; -- Run 50 times!
   ```
* **Result:** 1 query becomes **51 database queries**, creating severe database connection pool contention.
* **The Fix: DataLoader:** Use batching and caching libraries (e.g., Facebook's `DataLoader`) to coalesce 50 separate queries into a single batch query:
  ```sql
  SELECT * FROM orders WHERE user_id IN (1, 2, 3, ..., 50);
  ```

---

## 5. Advantages & Disadvantages

### Advantages
* **Zero Over-Fetching & Under-Fetching:** Clients download only the bytes they display on screen, critical for bandwidth-constrained mobile devices.
* **Consolidated Network Round-Trips:** Replaces 5 sequential REST calls with 1 unified GraphQL query.
* **Introspective Typed Schema:** The schema serves as live, machine-readable API documentation and enables IDE autocompletion.

### Disadvantages
* **HTTP Caching is Difficult:** Because all queries are sent as HTTP `POST` to `/graphql`, edge CDN caching cannot inspect URLs directly; requires client-side normalized caches (Apollo Client / Relay).
* **Vulnerability to Malicious Complex Queries:** An attacker can craft deeply nested recursive queries (`user { friends { friends { friends ... } } }`) to trigger Denial of Service (DoS). Servers must enforce **query complexity analysis** and depth limiting.
* **Complex Server Authorization:** Authorization checks must be implemented at the field and resolver level rather than whole URL routes.

---

## 6. Best Use Cases

* **Mobile-First Consumer Applications:** Apps where cell network bandwidth is constrained and screens require diverse data slices.
* **Backend-for-Frontend (BFF) Layers:** Aggregating data from dozens of internal REST and gRPC microservices into a single client gateway.
* **Rapid UI Experimentation:** Frontend teams add or remove fields from screens without waiting for backend engineers to deploy new API endpoints.

---

## 7. Related Topics & Next Steps

* [REST Architecture](./rest.md) — The conventional resource-oriented standard.
* [WebSockets](./websockets.md) — Underpins GraphQL real-time Subscriptions.
* [Communication Style Comparison](./communication-style-comparison.md) — Direct feature comparison matrix.

---

| ⬅️ Previous Topic | 🏠 Index | Next Topic ➡️ |
|:--- |:---:| ---:|
| [RPC and gRPC](./rpc-and-grpc.md) | [03. Networking Index](./README.md) | [WebSockets ➡️](./websockets.md) |
