# Imports the Google Cloud Translation library
from google.cloud import translate

project_id = "universityproject-335121"

def detect_language(text, project_id=project_id):
    """Detecting the language of a text string."""

    client = translate.TranslationServiceClient()

    location = "global"

    parent = f"projects/{project_id}/locations/{location}"

    # Detail on supported types can be found here:
    # https://cloud.google.com/translate/docs/supported-formats
    response = client.detect_language(
        content=text,
        parent=parent,
        mime_type="text/plain",  # mime types: text/plain, text/html
    )

    return response.languages[0].language_code

# Initialize Translation client
def translate_text(text, project_id="universityproject-335121"):
    """Translating Text."""

    client = translate.TranslationServiceClient()

    location = "global"

    parent = f"projects/{project_id}/locations/{location}"



    # Translate text from English to French
    # Detail on supported types can be found here:
    # https://cloud.google.com/translate/docs/supported-formats
    response = client.translate_text(
        request={
            "parent": parent,
            "contents": [text],
            "mime_type": "text/plain",  # mime types: text/plain, text/html
            "source_language_code": detect_language(text),
            "target_language_code": "ru",
        }
    )
    return response.translations[0].translated_text
