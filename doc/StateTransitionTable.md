# State Transition Table



```cpp
using FcmSttAction = std::function<void(const FcmMessage&)>;
```

```cpp
struct FcmSttTransition
{
FcmSttAction action;
std::string nextState;
};
```

