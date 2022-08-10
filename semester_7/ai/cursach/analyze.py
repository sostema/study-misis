from dostoevsky.tokenization import RegexTokenizer
from dostoevsky.models import FastTextSocialNetworkModel

# Функция которая предсказывает эмоциональную окраску текстового сообщения
def predict(message_text):
    # Текнизатор на основе регулярных выражений
    tokenizer = RegexTokenizer()
    # Модель для предсказания, которая основона на датасете FastTextSocialNetwork
    model = FastTextSocialNetworkModel(tokenizer=tokenizer)

    # Результат работы модели представляет из себя словарь из k наиболее
    # близких эмоциональных окрасок текста с их вероятностями
    result = model.predict([message_text], k=1)[0]
    return list(result.keys())[0]