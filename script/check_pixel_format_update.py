from generate_converter import pdf_url, dl_and_load_pdf, is_new_pdf_available
import sys

if __name__ == "__main__":
    online_pdf_date, data_int_key, data_str_key = dl_and_load_pdf(pdf_url)
    
    if is_new_pdf_available(online_pdf_date):
        sys.exit(1)
    else:
        sys.exit(0)