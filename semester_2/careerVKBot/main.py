import configparser

import vk_api
from vk_api.bot_longpoll import VkBotLongPoll, VkBotEventType
from vk_api.keyboard import VkKeyboard, VkKeyboardColor
from vk_api.utils import get_random_id

from Code.resume_generator import generate_template
from Code.sql_tools import SQLDataBase


def construct_keyboard():
    keyboard = VkKeyboard(one_time=True)
    keyboard.add_button('Трудоустройство', color=VkKeyboardColor.DEFAULT)
    keyboard.add_button('Составить резюме', color=VkKeyboardColor.POSITIVE)
    keyboard.add_line()  # Переход на вторую строку
    keyboard.add_button('Получить резюме', color=VkKeyboardColor.DEFAULT)
    return keyboard


def get_resume(user_id, database):
    user = database.get_user(user_id)
    resume = database.get_resume(user_id)
    context = {"phone": resume.phone, "email": resume.email,
               "full_name": " ".join([user.first_name, user.second_name, user.last_name]),
               "work_exp": resume.work_exp,
               "profession": resume.profession,
               "birthday": resume.birthday,
               "full_years": resume.full_years,
               "wanted_profession": resume.wanted_profession
               }
    return generate_template(context)


def main():
    states = dict()
    # Various states:
    # IDLE
    # {"RESUME": 0 }

    config = configparser.ConfigParser()
    config.read("./config.ini")
    token = config.get("credentials", "token")
    group_id = config.get("credentials", "group_id")

    vk_session = vk_api.VkApi(token=token)
    upload = vk_api.VkUpload(vk_session)
    vk = vk_session.get_api()

    database = SQLDataBase()

    keyboard = construct_keyboard()

    longpoll = VkBotLongPoll(vk_session, group_id=group_id)

    for event in longpoll.listen():
        if event.type == VkBotEventType.MESSAGE_NEW:
            event: vk_api.bot_longpoll.VkBotEvent = event

            states.setdefault(event.obj.from_id, 'IDLE')

            if event.obj.text == 'Начать':
                vk.messages.send(
                    user_id=event.obj.from_id,
                    random_id=get_random_id(),
                    keyboard=keyboard.get_keyboard(),
                    message="Привет! Я бот Центра Карьеры!"
                )
            elif event.obj.text == 'Трудоустройство':
                vk.messages.send(
                    user_id=event.obj.from_id,
                    random_id=get_random_id(),
                    keyboard=keyboard.get_keyboard(),
                    message="https://misis.ru/students/vacancies/"
                )
            elif event.obj.text == 'Составить резюме':
                states[event.obj.from_id] = {'RESUME': 0, 'USER_QUERY': database.User(),
                                             'RESUME_QUERY': database.Resume()}
            elif event.obj.text == 'Получить резюме':
                if database.get_resume(event.obj.from_id):
                    resume_document = get_resume(event.obj.from_id, database)
                    file = upload.document_message(resume_document, 'resume', peer_id=event.obj.from_id)
                    vk.messages.send(
                        user_id=event.obj.from_id,
                        random_id=get_random_id(),
                        attachment=file['doc']['url'][len('https://vk.com/'):],
                        keyboard=keyboard.get_keyboard(),
                        message="Вот ваше резюме!"
                    )
                else:
                    vk.messages.send(
                        user_id=event.obj.from_id,
                        random_id=get_random_id(),
                        keyboard=keyboard.get_keyboard(),
                        message="Сначала создайте резюме :)"
                    )

            else:
                user = event.obj.from_id
                if states[event.obj.from_id] != 'IDLE':
                    resume_state = states[user]['RESUME']
                    user_query = states[user]['USER_QUERY']
                    resume_query = states[user]['RESUME_QUERY']
                    if resume_state == 0:
                        user_query.first_name = event.obj.text
                    elif resume_state == 1:
                        user_query.second_name = event.obj.text
                    elif resume_state == 2:
                        user_query.last_name = event.obj.text
                    elif resume_state == 3:
                        resume_query.birthday = event.obj.text
                    elif resume_state == 4:
                        resume_query.full_years = event.obj.text
                    elif resume_state == 5:
                        resume_query.wanted_profession = event.obj.text
                    elif resume_state == 6:
                        resume_query.work_exp = event.obj.text
                    elif resume_state == 7:
                        resume_query.profession = event.obj.text
                    elif resume_state == 8:
                        resume_query.phone = event.obj.text
                    elif resume_state == 9:
                        resume_query.email = event.obj.text
                    states[user]['USER_QUERY'] = user_query
                    states[user]['RESUME_QUERY'] = resume_query
                    states[user]['RESUME'] += 1

            user = event.obj.from_id
            if states[user] != 'IDLE':
                resume_state = states[user]['RESUME']
                if resume_state == 0:
                    vk.messages.send(
                        user_id=user,
                        random_id=get_random_id(),
                        message="Введите имя: "
                    )
                elif resume_state == 1:
                    vk.messages.send(
                        user_id=user,
                        random_id=get_random_id(),
                        message="Введите фамилию: "
                    )
                elif resume_state == 2:
                    vk.messages.send(
                        user_id=user,
                        random_id=get_random_id(),
                        message="Введите отчество: "
                    )
                elif resume_state == 3:
                    vk.messages.send(
                        user_id=user,
                        random_id=get_random_id(),
                        message="Введите дату рождения: "
                    )
                elif resume_state == 4:
                    vk.messages.send(
                        user_id=user,
                        random_id=get_random_id(),
                        message="Введите количество полных лет: "
                    )
                elif resume_state == 5:
                    vk.messages.send(
                        user_id=user,
                        random_id=get_random_id(),
                        message="Введите желаемую профессию: "
                    )
                elif resume_state == 6:
                    vk.messages.send(
                        user_id=user,
                        random_id=get_random_id(),
                        message="Введите стаж работы: "
                    )
                elif resume_state == 7:
                    vk.messages.send(
                        user_id=user,
                        random_id=get_random_id(),
                        message="Введите профессию: "
                    )
                elif resume_state == 8:
                    vk.messages.send(
                        user_id=user,
                        random_id=get_random_id(),
                        message="Введите телефон: "
                    )
                elif resume_state == 9:
                    vk.messages.send(
                        user_id=user,
                        random_id=get_random_id(),
                        message="Введите электронную почту: "
                    )
                elif resume_state == 10:
                    user_query = states[user]['USER_QUERY']
                    resume_query = states[user]['RESUME_QUERY']
                    user_query.id = user
                    resume_query.user_id = user_query.id

                    database.add_user(user_query)
                    database.add_resume(resume_query)

                    resume_document = get_resume(event.obj.from_id, database)
                    file = upload.document_message(resume_document, 'resume', peer_id=event.obj.from_id)

                    states[user] = 'IDLE'

                    vk.messages.send(
                        user_id=user,
                        random_id=get_random_id(),
                        attachment=file['doc']['url'][len('https://vk.com/'):],
                        keyboard=keyboard.get_keyboard(),
                        message="Спасибо! Мы составили Вам резюме!"
                    )


if __name__ == '__main__':
    main()
