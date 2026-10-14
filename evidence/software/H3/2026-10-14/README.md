# H3 Tach 输入捕获软件证据（2026-10-14）

## 1. 范围

本目录记录 `TACH-00G` 关闭 PA0 低压电气准入后，Node A 最小 TIM2 原始输入捕获测试
构建。当前只完成离线实现、clean build 与静态复核，尚未刷写实板。

## 2. 文件与完整性

| 文件 | 大小 | SHA-256 | 内容 |
|---|---:|---|---|
| [`H3_NODE-A_TACH-01A_raw-capture-build_20261014.txt`](H3_NODE-A_TACH-01A_raw-capture-build_20261014.txt) | `4193` bytes | `22419728C5C4E08F81108715F0B1C99D2EC894FB5F80FB21CEEED75CD43602BD` | 变更边界、ISR/任务职责、构建结果、ELF 身份、栈记录和下一实板门禁 |

## 3. 当前结论

- 最小 TIM2_CH1 下降沿捕获测试镜像已成功构建；
- 日志只报告 raw edge/period/frequency evidence，明确保持 `ppr=TBD / rpm=INVALID`；
- TIM2 `ICFilter=0` 是本轮有意冻结，用来单独验证已经实测通过的硬件 RC，不等于最终
  数字滤波策略；
- 下一步必须先做 Node A 单板、风扇 12 V 断开的零边沿基线，再加入风扇电源做默认
  50% 捕获对照；在实板日志通过前，不宣称 TIM2、PPR 或 RPM 已通过。
