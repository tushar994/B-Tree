import pandas as pd
import math
import matplotlib.pyplot as plt
import numpy as np


data = pd.read_csv('generate_11.csv', delimiter=',')

print(data)


data.plot(use_index=True, y = ["with","without"])
plt.show()

fig, ax = plt.subplots()
data['with'].value_counts().plot(ax=ax, kind='bar')
plt.show()


fig, ax = plt.subplots()
data['without'].value_counts().plot(ax=ax, kind='bar')
plt.show()