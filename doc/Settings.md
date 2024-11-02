# Using settings

_The FCM framework provides a flexible way to configure components through a `settings` mechanism. This allows developers to pass configuration parameters as key-value pairs, enabling dynamic behavior and customization of components without modifying their source code._

## Overview of settings

- **Definition**: Settings are defined as a `std::map<std::string, std::any>`, allowing storage of various data types associated with string keys.
  
  ```cpp
  using FcmSettings = std::map<std::string, std::any>;
  ```

- **Purpose**: They enable the initialization and configuration of components with customizable parameters, facilitating reusable and modular component design.

## Defining settings for a Component

When creating a component, `settings` can be provided to configure its behavior. These settings are passed during the instantiation of the component and are stored as a constant reference within the base component class.

### Example: Configuring a Timeout

Suppose you have a functional component that requires a connection timeout value. You can define this setting as follows:

```cpp
FcmSettings settings;
settings["connectionTimeoutMs"] = 5000; // 5000 milliseconds
auto connectionMonitor = createComponent<ConnectionMonitor>("ConnectionMonitor", settings);
```

In this example:

- A `connectionTimeoutMs` key is set with a value of `5000` milliseconds.
- The `ConnectionMonitor` component is instantiated with these settings.

## Accessing and Using `Settings` in Components

Components can access these settings using the `setSetting()` template method provided by the `FcmBaseComponent` class. This method retrieves the value associated with a given key and assigns it to a state variable.

### Implementing `setSetting`

Within your component, define state variables that will hold the configuration values:

```cpp
class ConnectionMonitor : public FcmFunctionalComponent
{
public:
    ConnectionMonitor(const std::string& name, const FcmSettings& settings)
        : FcmFunctionalComponent(name, settings) {}

    void initialize() override 
    {
        FcmFunctionalComponent::initialize();
        setSetting<int>("connectionTimeoutMs", connectionTimeoutMs);
    }

private:
    int connectionTimeoutMs;
};
```

Here:

- `connectionTimeoutMs` is a state variable that stores the timeout value.
- `setSetting<int>` retrieves the value associated with `"connectionTimeoutMs"` from `settings` and assigns it to `connectionTimeoutMs`.

### Handling Missing or Invalid Settings

The `setSetting()` method throws a `std::runtime_error` if the key is not found or if the type does not match. This ensures that components are correctly configured.

```cpp
template <typename T>
void setSetting(const std::string& settingName, T& stateVariable) 
{
    try 
    {
        stateVariable = std::any_cast<T>(settings.at(settingName));
    }
    catch (const std::bad_any_cast& e)
    {
        throw std::runtime_error("Component: " + name + " settings error: " + e.what());
    }
}
```

## Best Practices for Using `Settings`

1. **Use Descriptive Keys**: Ensure that the keys in the `settings` map are clear and descriptive to avoid confusion.

2. **Type Safety**: Always use the correct type when setting and retrieving values. Mismatched types will result in runtime errors.

3. **Validation**: Validate settings during initialization to ensure they meet the component's requirements.

5. **Documentation**: Clearly document the expected settings for each component, including their types and default values.

## Conclusion

The `settings` functionality in the FCM framework offers a powerful way to configure components dynamically. By leveraging key-value pairs, developers can create highly customizable and reusable components, leading to more maintainable and scalable systems. Proper use of `settings` enhances component flexibility, allowing for easy adjustments and extensions without altering the underlying codebase.
