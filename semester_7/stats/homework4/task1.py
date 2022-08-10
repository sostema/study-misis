import re
import statsmodels.formula.api as sf
import statsmodels.api as sm
from scipy.stats import chi2_contingency
import pandas as pd
import numpy as np
df = pd.read_csv("./1_test_luggage_1.csv")

def get_features(data):
    result = []
    # Меняем значения зависимой переменной на 1 / 0
    data.is_prohibited[data.is_prohibited == 'Yes'] = 1
    data.is_prohibited[data.is_prohibited == 'No'] = 0
    # ЛОГИСТИЧЕСКАЯ РЕГРЕССИЯ
    logit_res = sf.glm('is_prohibited ~ C(type) + weight + length + width', data, family = sm.families.Binomial()).fit().summary()
    print(logit_res)
    coef_names = np.array([str(logit_res.tables[1][i][0]) for i in range(1, 6)])
    p_values = np.array([float(str(logit_res.tables[1][i][4])) for i in range(1, 6)])
    # Выбираем только те имена коэфициентов, для которых p_values < 0.05
    res1 = coef_names[p_values < 0.05]
    
    # Для номинативных зависимых переменных название коэфициентов будет отличаться от изначального
    # Например, 'C(type)[T.Suitcase]' вместо 'type'. Поэтому мы приводим их в изначальный вид
    result = [re.findall(name, r) for r in res1 for name in ['weight', 'length', 'width', 'type']]
    result = list(np.array([r for r in result if r]).ravel())
    
    # Если в массив результатов не пустой, возвращаем его, иначе - текст
    return result if len(result) else 'Prediction makes no sense'

get_features(df)