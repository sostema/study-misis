import logging

from telegram import Update, ForceReply
from telegram.ext import Updater, CommandHandler, MessageHandler, Filters, CallbackContext
from analyze import predict

# Добавление логгирования программы
logging.basicConfig(
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO
)

logger = logging.getLogger(__name__)

# Токен телеграмм бота
TOKEN=''

# Функция которая срабатывает при первом сообщении боту
def start(update: Update, context: CallbackContext) -> None:
    """Send a message when the command /start is issued."""
    user = update.effective_user
    update.message.reply_markdown_v2(
        f'Привет! Пришли мне сообщение и я отвечу, какая у него окраска!',
        reply_markup=ForceReply(selective=True),
    )

# Функция которая срабатывает, когда пользователь отправляет сообщение боту
# Возвращает описание эмоциональной окраски текста на основе предсказания
# основного алгоритма
def echo(update: Update, context: CallbackContext) -> None:
    """Echo the user message."""
    result = predict(update.message.text)
    if result == "speech":
        result = "Это обычная разговорная речь"
    elif result == "negative":
        result = "Этот текст имеет негативную окраску"
    elif result == "positive":
        result = "Этот текст имеет позитивную окраску"
    else:
        result = "Этот текст не имеет эмоциональной окраски"
    update.message.reply_text(result, quote=True)

# Основная функция телеграм бота
def main() -> None:
    # Создаёт объект обновления для корректной работы бота
    updater = Updater(TOKEN)

    # Создаёт объект диспетчера для регистрации обработчиков
    dispatcher = updater.dispatcher

    # Добавляет обработчик старта диалога в диспетчер
    dispatcher.add_handler(CommandHandler("start", start))

    # Добавляет обработчик входящих сообщений в диспетчер
    dispatcher.add_handler(MessageHandler(Filters.text & ~Filters.command, echo))

    # Запускает работу бота
    updater.start_polling()

    updater.idle()


if __name__ == '__main__':
    main()