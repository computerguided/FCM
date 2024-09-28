# Guide to Deriving States, Transitions, and Choice-Points from a Component's Role and Behavior

_Understanding how to translate a component's role and behavior into states, transitions, and choice-points is crucial for designing robust and maintainable systems. This guide walks you through the process step-by-step, leveraging the Functional Components Method (FCM) to structure your component's behavior effectively._

---

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

| State | Description |
| --- | --- |
| Idle | Waiting for requests. |
| Processing | Forwarding a valid request to the database. |

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

<table>
  <thead>
    <tr>
      <th>Current State</th>
      <th>Interface</th>
      <th>Message</th>
      <th>Action</th>
      <th>Next State</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td rowspan="2">Idle</td>
      <td rowspan="2">Database</td>
      <td>AddInd</td>
      <td>
        - Set `clientIndex = 0`<br>
        - Set `operationType = Add`
      </td>
      <td>Is Id Valid?</td>
    </tr>
    <tr>
      <td>RemoveInd</td>
      <td>
        - Set `clientIndex = 0`<br>
        - Set `operationType = Remove`
      </td>
      <td>Is Id Valid?</td>
    </tr>
    <tr>
      <td rowspan="2">Is Id Valid?</td>
      <td rowspan="2">Logical</td>
      <td>Yes</td>
      <td>Resend last received message</td>
      <td>Processing</td>
    </tr>
    <tr>
      <td>No</td>
      <td>Ignore and log invalid id</td>
      <td>Idle</td>
    </tr>
    <tr>
      <td>Processing</td>
      <td>Database</td>
      <td>ReadyInd</td>
      <td>
        - Update `databaseEntries` from `ReadyInd`<br>
      </td>
      <td>Was Operation Remove?</td>
    </tr>
    <tr>
      <td rowspan="2">Was Operation Remove?</td>
      <td rowspan="2">Logical</td>
      <td>Yes</td>
      <td>No action needed</td>
      <td>Is Database Empty?</td>
    </tr>
    <tr>
      <td>No</td>
      <td>No action needed</td>
      <td>Idle</td>
    </tr>
    <tr>
      <td rowspan="2">Is Database Empty?</td>
      <td rowspan="2">Logical</td>
      <td>Yes</td>
      <td>Perform actions when database is empty</td>
      <td>Idle</td>
    </tr>
    <tr>
      <td>No</td>
      <td>No action needed</td>
      <td>Idle</td>
    </tr>
  </tbody>
</table>

---

## Step 7: Construct the State Diagram

**Objective**: Visualize the states, transitions, and choice-points to ensure correctness and completeness.

- **Include Choice-Points**: Represent choice-points as decision nodes ending with a question mark.
- **Show Conditional Paths**: Indicate the outcomes of choice-point evaluations ("Yes", "No" branches).

**Example**:

![](https://www.plantuml.com/plantuml/img/XP31Ikn044NtzHMNMJx81xmiT0PKo4P5GH5nKKwLsfXqXwuUPHJ_NRk7cM0JS1C8TVlSKxMqn35jXfvUd2Xq6DCMehQERamkU29xTNxao0DMCOPtV19m5dowTX-A__H5TF_l0KslJwavGYUz4kKlclnhdTnmxmGjJZZn8vksAv7Eon-XAnt2c-QS9KwAHMVe12Ltl03Rz8ne8cJUPGnhDNF-6ItkrAfnwMrn7cWAbi4jsyMeaVFUk_hVGYpRxBCN-edZaCVkGJ6R0H7DmClgU_vfkNpndSbMujDD3dueWD9VKudgcebM-kfnLRPKBvkX_mO0)

*Note: The diagram illustrates transitions between `Idle`, `Is Id Valid?`, `Processing`, `Was Operation Remove?`, and `Is Database Empty?` states, including handling of valid and invalid `id`s.*

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
    FCM_ADD_CHOICE_POINT("Is Id Valid?",
        auto dataReq = FCM_CAST_LAST_RECEIVED_MESSAGE(DataRequestMessage);
        return dataReq->id == myId;
    );

    // Choice-Point: Was Operation Remove?
    FCM_ADD_CHOICE_POINT("Was Operation Remove?",
        return operationType == Operation::Remove;
    );

    // Choice-Point: Is Database Empty?
    FCM_ADD_CHOICE_POINT("Is Database Empty?",
        return databaseEntries == 0;
    );
}
```

- **Explanation**:
  - **"Is Id Valid?"**: Checks if the `id` in the last received message matches `myId`.
  - **"Was Operation Remove?"**: Determines if the last operation was a removal.
  - **"Is Database Empty?"**: Evaluates whether the `databaseEntries` count is zero.

### Define Transitions:

```cpp
void setTransitions() override
{
    // From Idle to Is Id Valid? on AddInd
    FCM_ADD_TRANSITION("Idle", Database, AddInd, "Is Id Valid?",
        clientIndex = 0;
        operationType = Operation::Add; // Track operation type
    );

    // From Idle to Is Id Valid? on RemoveInd
    FCM_ADD_TRANSITION("Idle", Database, RemoveInd, "Is Id Valid?",
        clientIndex = 0;
        operationType = Operation::Remove; // Track operation type
    );

    // From Is Id Valid? to Processing on Logical:Yes
    FCM_ADD_TRANSITION("Is Id Valid?", Logical, Yes, "Processing",
        resendLastReceivedMessage();
    );

    // From Is Id Valid? to Idle on Logical:No
    FCM_ADD_TRANSITION("Is Id Valid?", Logical, No, "Idle",
        // Handle invalid id (e.g., log or ignore)
    );

    // From Processing to Was Operation Remove? on ReadyInd
    FCM_ADD_TRANSITION("Processing", Database, ReadyInd, "Was Operation Remove?",
        auto readyInd = FCM_CAST_MESSAGE(message, ReadyIndMessage);
        databaseEntries = readyInd->entriesCount; // Update entries count
    );

    // From Was Operation Remove? to Is Database Empty? on Logical:Yes
    FCM_ADD_TRANSITION("Was Operation Remove?", Logical, Yes, "Is Database Empty?",
        // No action needed
    );

    // From Was Operation Remove? to Idle on Logical:No
    FCM_ADD_TRANSITION("Was Operation Remove?", Logical, No, "Idle",
        // No action needed
    );

    // From Is Database Empty? to Idle on Logical:Yes
    FCM_ADD_TRANSITION("Is Database Empty?", Logical, Yes, "Idle",
        // Perform actions when database is empty
    );

    // From Is Database Empty? to Idle on Logical:No
    FCM_ADD_TRANSITION("Is Database Empty?", Logical, No, "Idle",
        // No action needed
    );
}
```

- **Explanation**:
  - **Tracking Operation Type**:
    - The state variable `operationType` tracks whether the current operation is an `Add` or `Remove`.
  - **From `Processing` State**:
    - After receiving `ReadyInd`, move to the choice-point "Was Operation Remove?" to decide the next state based on the operation type.
  - **Using Choice-Points**:
    - Avoid using if-statements in actions; utilize choice-points to make decisions about state transitions.

### Implement State Variables:

```cpp
// State variables
enum class Operation { None, Add, Remove };
Operation operationType = Operation::None;
int databaseEntries = 0;
```

- **Explanation**:
  - `operationType`: Tracks the type of operation being processed.
  - `databaseEntries`: Stores the current number of entries in the database.

---

## Conclusion

By systematically translating a component's role and behavior into defined states, transitions, and choice-points—as closed questions ending with a question mark—you create a clear and maintainable structure for your component. This process ensures all scenarios are accounted for and the component behaves predictably in response to various messages and events.

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

This guide provided a comprehensive approach to derive states, transitions, and choice-points from a component's role and behavior. By following the steps outlined:

1. **Define the Role and Behavior**: Understand what the component does.
2. **Identify Use Cases and Scenarios**: Break down functionality into specific situations.
3. **Determine the States**: Identify distinct modes the component can be in.
4. **Identify Events and Messages**: Recognize inputs that cause transitions.
5. **Determine Choice-Points**: Identify decision points with conditions.
6. **Define Transitions**: Specify how the component moves between states.
7. **Construct the State Diagram**: Visualize the states and transitions.
8. **Implement in Code**: Translate everything into code using the FCM framework, ensuring proper use of choice-points.

By meticulously applying these steps, you design components that are robust, maintainable, and perfectly aligned with their intended roles within the system.

---

By following this guide, developers can create well-structured components that enhance the overall quality and maintainability of the software system.