import numpy as np
import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

sns.set(style="white", context="talk")
f, (ax1, ax2) = plt.subplots(1, 2, figsize=(8, 4), sharex=True)

ds = pd.DataFrame([["SuperString", 4.27], ["std::string", 456.96]], columns=["#", ""])
ds = pd.DataFrame({"#": ["SuperString", "std::string"], "Memory Consumption": [4.27, 456.96], "Number of Instructions": [223, 488]})

mg = sns.barplot(x="#", y="Memory Consumption", data=ds, ax=ax1)
ax1.set(xlabel='', ylabel='Memory Consumption (in MB)')

for index, row in ds.iterrows():
    mg.text(row.name, row["Memory Consumption"], row["Memory Consumption"], color='black', ha="center")
    
mg2 = sns.barplot(x="#", y="Number of Instructions", data=ds, ax=ax2)
ax2.set(xlabel='', ylabel='Number of Instructions (in Mi)')

for index, row in ds.iterrows():
    mg2.text(row.name, row["Number of Instructions"], row["Number of Instructions"], color='black', ha="center")

sns.despine(bottom=True)
plt.setp(f.axes, yticks=[])
plt.tight_layout(h_pad=3)

plt.savefig("text_line_split_bench.png")
