# Guide to Deriving States, Transitions, and Choice-Points from a Component's Role and Behavior

_Understanding how to translate a component's role and behavior into states, transitions, and choice-points is crucial for designing robust and maintainable systems. This guide walks you through the process step-by-step, leveraging the Functional Components Method (FCM) to effectively structure your component's behavior._

## Introduction

A functional component in software encapsulates specific functionality and interacts with other components via well-defined interfaces. Defining the states and transitions of a component helps in:

- **Clarifying behavior**: Makes the component's actions explicit under various conditions.
- **Facilitating maintenance**: Simplifies understanding and modifying the component's behavior.
- **Enabling reuse**: Promotes modularity, making components reusable in different contexts.

---

## Step 1: Define the Component's Role and Behavior

**Objective**: Clearly articulate what the component is responsible for in the system.

- **Describe the functionality**: What does the component do?
- **Identify responsibilities**: What tasks is it responsible for completing?
- **Specify interactions**: How does it interact with other components or external systems?

**Example**:

Let's consider a `DatabaseManager` component:

- **Role**: Manages database operations such as adding and removing entries.
- **Behavior**:
  - Receives requests to add or remove data.
  - Validates requests based on an `id` parameter.
  - Forwards valid requests to the database.
  - Handles database responses and checks if further actions are needed (e.g., checking if the database is empty after a removal).

---

## Step 2: Identify Use Cases and Scenarios

**Objective**: Break down the component's behavior into specific situations it must handle.

- **List use cases**: Different functionalities or features the component provides.
- **Outline scenarios**: Step-by-step sequences within each use case.

**Example**:

For the `DatabaseManager`:

- **Use Case 1**: Adding data to the database.
  - Scenario: Receives an `AddInd` message → Validates `id` → Forwards to database → Handles `ReadyInd` response.
- **Use Case 2**: Removing data from the database.
  - Scenario: Receives a `RemoveInd` message → Validates `id` → Forwards to database → Handles `ReadyInd` response → Checks if database is empty.
- **Use Case 3**: Handling invalid `id` in requests.
  - Scenario: Receives an invalid request → Ignores or logs the issue.

---

## Step 3: Determine the States

**Objective**: Identify the distinct conditions or modes the component can be in during its lifecycle.

- **Define states based on behavior**: Each state represents a stage in processing or waiting.
- **Ensure clarity**: State names should reflect the component's activity.

**Example**:

States for the `DatabaseManager`:

| State       | Description                |
|:-------------|:---------------|
| Idle        | Waiting for requests.      |
| Processing  | Forwarding a valid request to the database. |

---

## Step 4: Identify Events and Messages

**Objective**: Recognize the inputs (messages/events) that cause state transitions.

- **List all possible messages**: Include those received from interfaces and internal triggers.
- **Associate messages with states**: Determine which messages are relevant in each state.

**Example**:

Messages for the `DatabaseManager`:

- **External Messages**:
  - `Database:AddInd`
  - `Database:RemoveInd`
  - `Database:ReadyInd`

Note that the `Logical` interface is present by default in FCM and is used to handle the conditions in choice-points.

---

## Step 5: Determine Choice-Points

**Objective**: Identify decision points where the component's next action depends on evaluating a condition.

### What Are Choice-Points?

Choice-points are decision nodes in the state machine where the next state depends on the evaluation of a condition or expression. They are formulated as closed questions that can be answered with "Yes" or "No" and end with a question mark to distinguish them from states.

### How to Identify Choice-Points:

1. **Analyze Scenarios for Conditional Logic**:
   - Look for points where different actions are taken based on certain conditions.
   - Examples include validation checks, availability of resources, or responses from other components.

2. **Identify Conditions That Affect Flow**:
   - Conditions based on message content (e.g., `id` matches, status codes).
   - Conditions based on internal state (e.g., counters, flags).

3. **Determine the Outcome of Each Condition**:
   - Specify what happens if the condition is true or false.
   - Ensure all possible outcomes are accounted for.

4. **Name Choice-Points Clearly**:
   - Use descriptive names that are closed questions ending with a question mark.
   - Examples: "Is Id Valid?", "Is Database Empty?", "Was Operation Remove?"

**Example**:

In the `DatabaseManager` component:

- **Choice-Point 1**: "Is Id Valid?"
  - **Condition**: After receiving an `AddInd` or `RemoveInd` message, check if the `id` in the message matches `myId`.
  - **Yes**: Proceed to process the message.
  - **No**: Ignore the message and return to the Idle state.

- **Choice-Point 2**: "Was Operation Remove?"
  - **Condition**: After processing a database operation, check if the operation was a removal.
  - **Yes**: Check if the database is empty.
  - **No**: Return to Idle state.

- **Choice-Point 3**: "Is Database Empty?"
  - **Condition**: Check if the database has zero entries.
  - **Yes**: Perform actions when the database is empty.
  - **No**: No action needed; return to Idle state.

---

## Step 6: Define Transitions

**Objective**: Specify how the component moves between states in response to messages and conditions.

- **Create a State Transition Table**: For each state, list possible messages, the resulting actions, and the next state.
- **Include Choice-Points**: Incorporate choice-points into your transitions to handle decision logic.

**Example**:

### State Transition Table for `DatabaseManager`

| Current State       | Interface  | Message     | Action                               | Next State         |
|:-----|:----|:-------------|:-------|:--------------------|
| Idle                | Database   | AddInd      | Set `clientIndex = 0`, `operationType = Add` | Is Id Valid?       |
| Idle                | Database   | RemoveInd   | Set `clientIndex = 0`, `operationType = Remove` | Is Id Valid?       |
| Is Id Valid?        | Logical    | Yes         | Resend last received message         | Processing         |
| Is Id Valid?        | Logical    | No          | Ignore and log invalid id            | Idle               |
| Processing          | Database   | ReadyInd    | Update `databaseEntries` from `ReadyInd` | Was Operation Remove? |
| Was Operation Remove? | Logical  | Yes         | No action needed                     | Is Database Empty? |
| Was Operation Remove? | Logical  | No          | No action needed                     | Idle               |
| Is Database Empty?  | Logical    | Yes         | Perform actions when database is empty | Idle             |
| Is Database Empty?  | Logical    | No          | No action needed                     | Idle               |

---

## Step 7: Construct the State Diagram

**Objective**: Visualize the states, transitions, and choice-points to ensure correctness and completeness.

- **Include Choice-Points**: Represent choice-points as decision nodes ending with a question mark.
- **Show Conditional Paths**: Indicate the outcomes of choice-point evaluations ("Yes", "No" branches).

---

## Step 8: Implement the States, Transitions, and Choice-Points in Code

**Objective**: Translate the defined states, transitions, and choice-points into code using the FCM framework.

### Define States:

```cpp
void setStates() override
{
    states =
    {
        "Idle",
        "Processing",
        "Is Database Empty?"
    };
}
```

### Define Choice-Points:

```cpp
void setChoicePoints() override
{
    // Choice-Point: Is Id Valid?
    addChoicePoint("Is Id Valid?", [this]()
    {
        auto dataReq = castLastReceivedMessage<Commands::DataReq>();
        return dataReq->id == myId;
    });

    // Choice-Point: Was Operation Remove?
    addChoicePoint("Was Operation Remove?", [this]()
    {
        return operationType == Operation::Remove;
    });

    // Choice-Point: Is Database Empty?
    addChoicePoint("Is Database Empty?", [this]()
    {
        return databaseEntries == 0;
    });
}
```

### Define Transitions:

```cpp
void setTransitions() override
{
    // From Idle to Is Id Valid? on AddInd
    addTransition("Idle", Database, AddInd, "Is Id Valid?", [this](const std::shared_ptr<FcmMessage>& msg)
    {
        clientIndex = 0;
        operationType = Operation::Add;
    });

    // From Idle to Is Id Valid? on RemoveInd
    addTransition("Idle", Database, RemoveInd, "Is Id Valid?", [this](const std::shared_ptr<FcmMessage>& msg)
    {
        clientIndex = 0;
        operationType = Operation::Remove;
    });

    // From Is Id Valid? to Processing on Logical:Yes
    addTransition("Is Id Valid?", Logical, Yes, "Processing", [this](const std::shared_ptr<FcmMessage>& msg)
    {
        resendLastReceivedMessage();
    });

    // From Is Id Valid? to Idle on Logical:No
    addTransition("Is Id Valid?", Logical, No, "Idle", [this](const std::shared_ptr<FcmMessage>& msg)
    {
        // Handle invalid id (e.g., log or ignore)
        // ...
    });

    // From Processing to Was Operation Remove? on ReadyInd
    addTransition("Processing", Database, ReadyInd, "Was Operation Remove?", [this](const std::shared_ptr<FcmMessage>& msg)
    {
        auto readyInd = castLastReceivedMessage<Database::ReadyInd>();
        databaseEntries = readyInd->entriesCount;
    });

    // From Was Operation Remove? to Is Database Empty? on Logical:Yes
    addTransition("Was Operation Remove?", Logical, Yes, "Is Database Empty?", [this](const std::shared_ptr<FcmMessage>& msg)
    {
        // No action needed
        (void)this;
    });

    // From Was Operation Remove? to Idle on Logical:No
    addTransition("Was Operation Remove?", Logical, No, "Idle", [this](const std::shared_ptr<FcmMessage>& msg)
    {
        // No action needed
        (void)this;
    });

    // From Is Database Empty? to Idle on Logical:Yes
    addTransition("Is Database Empty?", Logical, Yes, "Idle", [this](const std::shared_ptr<FcmMessage>& msg)
    {
        // Perform actions when database is empty
        // ...
    });

    // From Is Database Empty? to Idle on Logical:No
    addTransition("Is Database Empty?", Logical, No, "Idle", [this](const std::shared_ptr<FcmMessage>& msg)
    {
        // No action needed
        (void)this;
    });
}
```

---

## Conclusion

By systematically translating a component's role and behavior into defined states, transitions, and choice-points, you create a clear and maintainable structure for your component. This process ensures all scenarios are accounted for and the component behaves predictably in response to various messages and events.

Adopting this method provides several benefits:

- **Improved Clarity**: Both the code and documentation reflect the component's intended behavior.
- **Ease of Maintenance**: Changes to behavior can be made by updating states, transitions, or choice-points without impacting unrelated parts.
- **Enhanced Reliability**: Well-defined states, transitions, and decision points reduce the likelihood of unexpected behavior.

---

## Additional Tips

- **Use Descriptive Names**:
  - Choose clear and descriptive names for states and choice-points to convey their purpose.
  - Ensure choice-points are formulated as closed questions ending with a question mark.

- **Document Conditions**:
  - Clearly document the conditions evaluated in choice-points for future reference.

- **Handle All Outcomes**:
  - Ensure all possible outcomes of a condition are handled to prevent undefined behavior.

- **Avoid If-Statements in Actions**:
  - Do not include conditional logic in the action part of transitions. Instead, use choice-points to handle decisions.

- **Update Messages as Needed**:
  - If existing messages lack necessary information (e.g., `entriesCount` in `ReadyInd`), modify them to include required data.

---

## Summary

This guide provides a comprehensive approach to derive states, transitions, and choice-points from a component's role and behavior. By following the steps outlined:

1. **Define the Role and Behavior**: Understand what the component does.
2. **Identify Use Cases and Scenarios**: Break down functionality into specific situations.
3. **Determine the States**: Identify distinct modes the component can be in.
4. **Identify Events and Messages**: Recognize inputs that cause transitions.
5. **Determine Choice-Points**: Identify decision points with conditions.
6. **Define Transitions**: Specify how the component moves between states.
7. **Construct the State Diagram**: Visualize the states and transitions.
8. **Implement in Code**: Translate everything into code using the FCM framework, ensuring proper use of choice-points.

By meticulously applying these steps, you design components that are robust, maintainable, and perfectly aligned with their intended roles within the system, ultimately enhancing the overall quality and maintainability of the software system.