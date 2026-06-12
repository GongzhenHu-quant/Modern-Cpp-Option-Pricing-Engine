import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

sns.set_theme(style="darkgrid")
plt.rcParams['font.sans-serif'] = ['SimHei']  
plt.rcParams['axes.unicode_minus'] = False    

try:
    df = pd.read_csv("comparison_results.csv")
except FileNotFoundError:
    print("错误：找不到 comparison_results.csv 文件！")
    exit()

fig, axes = plt.subplots(2, 2, figsize=(15, 12))
(ax1, ax2), (ax3, ax4) = axes

lw = 2.5

# ---- 图 1：期权价格曲线对比 ----
sns.lineplot(data=df, x="Spot", y="EuroPrice", ax=ax1, color="royalblue", linewidth=lw, label="欧式看涨")
sns.lineplot(data=df, x="Spot", y="AsianPrice", ax=ax1, color="crimson", linewidth=lw, label="亚式看涨")
ax1.set_title("1. 欧式与亚式期权价格曲线对比", fontsize=11, fontweight='bold')
ax1.set_xlabel("标的资产现价")
ax1.set_ylabel("期权价格")
ax1.axvline(x=200, color="gray", linestyle="--")
ax1.legend(loc="upper left")

# ---- 图 2：95% VaR 风险对比 ----
sns.lineplot(data=df, x="Spot", y="EuroVaR", ax=ax2, color="orange", linewidth=lw, label="欧式 95% VaR")
sns.lineplot(data=df, x="Spot", y="AsianVaR", ax=ax2, color="firebrick", linewidth=lw, label="亚式 95% VaR")
ax2.set_title("2. 95% 置信度下 VaR 风险对比", fontsize=11, fontweight='bold')
ax2.set_xlabel("标的资产现价")
ax2.set_ylabel("VaR 损失额度")
ax2.axvline(x=200, color="gray", linestyle="--")
ax2.legend(loc="upper left")

# ---- 图 3：95% CVaR 条件期望损失对比 ----
sns.lineplot(data=df, x="Spot", y="EuroCVaR", ax=ax3, color="darkorange", linewidth=lw, label="欧式 95% CVaR")
sns.lineplot(data=df, x="Spot", y="AsianCVaR", ax=ax3, color="darkred", linewidth=lw, label="亚式 95% CVaR")
ax3.set_title("3. 95% 置信度下 CVaR 期望对比", fontsize=11, fontweight='bold')
ax3.set_xlabel("标的资产现价")
ax3.set_ylabel("CVaR 期望损失")
ax3.axvline(x=200, color="gray", linestyle="--")
ax3.legend(loc="upper left")

# ---- 图 4：欧式自身的尾部裂口 vs 亚式自身的尾部裂口 ----
# 这里计算的是 (CVaR - VaR)，衡量的是一旦跌破 VaR 边界，尾部到底有多“肥”
df['Euro_Tail_Gap'] = df['EuroCVaR'] - df['EuroVaR']
df['Asian_Tail_Gap'] = df['AsianCVaR'] - df['AsianVaR']

sns.lineplot(data=df, x="Spot", y="Euro_Tail_Gap", ax=ax4, color="purple", linewidth=lw, label="欧式尾部裂口 (CVaR - VaR)")
sns.lineplot(data=df, x="Spot", y="Asian_Tail_Gap", ax=ax4, color="darkcyan", linewidth=lw, label="亚式尾部裂口 (CVaR - VaR)")
ax4.set_title("4. 衍生品卖方的尾部风险裂口对比", fontsize=11, fontweight='bold')
ax4.set_xlabel("标的资产现价")
ax4.set_ylabel("风险裂口宽度")
ax4.axvline(x=200, color="gray", linestyle="--")
ax4.legend(loc="upper left")

plt.suptitle("欧式与亚式期权价格和风险对比", fontsize=14, fontweight='bold', y=0.96)
plt.tight_layout()
print("正在生成全维度风控四宫格可视化...")
plt.show()
