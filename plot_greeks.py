import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

sns.set_theme(style="darkgrid")
plt.rcParams['font.sans-serif'] = ['SimHei']  # 支持中文
plt.rcParams['axes.unicode_minus'] = False    # 支持负号

# 读取希腊字母数据
try:
    df = pd.read_csv("greeks_sensitivity.csv")
except FileNotFoundError:
    print("错误：找不到 greeks_sensitivity.csv 文件！请先运行 C++ 程序。")
    exit()

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
lw = 2.5 # 统一线宽

# ---- 图 1：Delta (一阶敏感度) 曲线对比 ----
sns.lineplot(data=df, x="Spot", y="EuroDelta", ax=ax1, color="royalblue", linewidth=lw, label="欧式 Delta")
sns.lineplot(data=df, x="Spot", y="AsianDelta", ax=ax1, color="crimson", linewidth=lw, label="亚式 Delta")
ax1.set_title("期权 Delta 对比", fontsize=12, fontweight='bold')
ax1.set_xlabel("标的资产现价 (Spot Price)", fontsize=10)
ax1.set_ylabel("Delta 值", fontsize=10)
ax1.axvline(x=200, color="gray", linestyle="--", label="行权价 (K=200)")
ax1.legend(loc="upper left")

# ---- 图 2：Gamma (二阶凸性风险) 曲线对比 ----
sns.lineplot(data=df, x="Spot", y="EuroGamma", ax=ax2, color="orange", linewidth=lw, label="欧式 Gamma")
sns.lineplot(data=df, x="Spot", y="AsianGamma", ax=ax2, color="darkred", linewidth=lw, label="亚式 Gamma")
ax2.set_title("期权 Gamma 对比", fontsize=12, fontweight='bold')
ax2.set_xlabel("标的资产现价 (Spot Price)", fontsize=10)
ax2.set_ylabel("Gamma 值", fontsize=10)
ax2.axvline(x=200, color="gray", linestyle="--")
ax2.legend(loc="upper right")

plt.suptitle("欧式与亚式期权希腊字母 (Greeks) 敏感度全景图", fontsize=14, fontweight='bold')
plt.tight_layout()
print("正在生成希腊字母全景对比图...")
plt.show()
