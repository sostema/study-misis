import os

from docxtpl import DocxTemplate


def generate_template(context):
    f = open('Data/resume_template.docx', 'rb')
    doc = DocxTemplate(f)
    doc.render(context)
    if os.path.exists("Data/Resumes/generated_doc.docx"):
        os.remove("Data/Resumes/generated_doc.docx")
    doc.save("Data/Resumes/generated_doc.docx")
    f.close()
    return "Data/Resumes/generated_doc.docx"
