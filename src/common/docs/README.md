# 模块级共享公共模块

## 统一错误码注册管理

在 `config/error_codes.json` 维护各个模块的错误码，
重新执行build后，会自动在common下生成 `error_code.h/.cpp`。

示例如下
```json
{
    "modules": [
      {
        "name": "COMMON",
        "id": 0,
        "submodules": [
          {
            "name": "SYSTEM",
            "id": 0,
            "errors": [
              { "name": "OK", "code": 0, "msg": "No error" },
              { "name": "UNKNOWN", "code": 1, "msg": "Unknown error" },
              { "name": "INVALID_PARAM", "code": 2, "msg": "Invalid parameter" }
            ]
          },
          {
            "name": "MODULE1",
            "id": 1,
            "errors": [
              { "name": "OK", "code": 0, "msg": "No error" }
            ]
          },
          {
            "name": "MODULE2",
            "id": 2,
            "errors": [
              { "name": "OK", "code": 0, "msg": "No error" }
            ]
          }
        ]
      }
    ]
  }
```


```cpp
sx_error_code_to_str(SX_ERR_SEEKER_COMMON_OK); // 将错误码转换为const char *
```
