import urllib.request
import os
from pathlib import Path
from pypdf import PdfReader
import argparse
import sys

pdf_url = 'https://www.emva.org/wp-content/uploads/GenICamPixelFormatValues.pdf'

src_dir = os.path.abspath(os.path.dirname(__file__))
root_dir = Path(src_dir).parent.absolute()

python_dir = os.path.join(root_dir, 'gendc_python', 'genicam')
cpp_dir = os.path.join(root_dir, 'gendc_cpp', 'genicam')

def dl_pdf(pdf_url):
    try:
        data_pdf = os.path.join(src_dir, 'data.pdf')
        print('DL {} ...'.format(pdf_url))
        urllib.request.urlretrieve(pdf_url, data_pdf)
    except:
        print('Failed to DL {}'.format(pdf_url))
        sys.exit(1)
    return data_pdf

def dl_and_load_pdf(pdf_url):
    # DL pdf from emva
    pdf_path = dl_pdf(pdf_url)

    print('Parse {} ...'.format(pdf_path))
    data_int_key = {}
    data_str_key = {}
    try:
        reader = PdfReader(pdf_path)
        for page in reader.pages:
            lines = page.extract_text().split('\n')
            for line in lines:
                columns = line.split(' ', 3)
                if len(columns) == 4 and columns[1].startswith('0x'):
                    data_int_key[int(columns[1], 16)] = columns[0]
                    data_str_key[columns[0]] = int(columns[1], 16)
                elif len(columns) == 1 and len(columns[0].split('-'))==3:
                    online_pdf_date = columns[0]
                    print('  line \"{}\" is skipped to convert'.format(line))
                else:
                    print('  line \"{}\" is skipped to convert'.format(line))
    except:
        print('Failed to load {}'.format(pdf_path))
        sys.exit(1)

    return online_pdf_date, data_int_key, data_str_key

def is_new_pdf_available(online_pdf_date):
    current_pdf_date = ''

    if not os.path.isfile(os.path.join(python_dir, 'pfnc_mapping.py')):
        return True

    with open(os.path.join(python_dir, 'pfnc_mapping.py'), 'r') as f:
        first_line = f.readlines()[0]
        if first_line.startswith('#'):
            current_pdf_date = first_line.split('[')[1].split(']')[0]

    if current_pdf_date != online_pdf_date:
        print('The version date of map/dictionary in the repository: {}'.format(current_pdf_date))
        print('The version date of GenICamPixelFormatValues.pdf    : {}'.format(online_pdf_date))
        return True
    else:
        # no need to generate a new one unless it is forced
        print('The version date of map/dictionary in the repository is up-to-date: {}'.format(current_pdf_date))
        return False

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="generate pfnc_converter dictionary script")
    parser.add_argument('-f', '--force', action='store_true', help='Force generate script by overwriting')
    argvs = parser.parse_args()
    force_generate = argvs.force

    # Get content of pdf
    online_pdf_date, data_int_key, data_str_key = dl_and_load_pdf(pdf_url)

    filename = 'pfnc_mapping'
    python_str = '# {}.py generated based on {} [{}]\n\n'.format(filename, pdf_url, online_pdf_date)
    cpp_str = '#ifndef {}_H\n'.format(filename.upper())
    cpp_str += '#define {}_H\n'.format(filename.upper())
    cpp_str += '// {}.h generated based on {} [{}]\n\n'.format(filename, pdf_url, online_pdf_date)
    cpp_str += '#include <iostream>\n'
    cpp_str += '#include <map>\n'
    cpp_str += '#include <string>\n\n'

    if not is_new_pdf_available(online_pdf_date) and not force_generate:
        sys.exit(0)

    if force_generate:
        print('Generate headers with latest pdf...')
        try:
            for obj_name in ['data_int_key', 'data_str_key']:
                
                python_str += obj_name + ' = { \\\n'
                if obj_name == 'data_int_key':
                    cpp_str += 'std::map<int32_t, std::string> ' + obj_name + ' {\n'
                elif obj_name == 'data_str_key':
                    cpp_str += 'std::map<std::string, int32_t> ' + obj_name + ' {\n'

                for key in eval(obj_name):
                    if obj_name == 'data_int_key':
                        python_str += '    {} : \'{}\',\n'.format(key, eval(obj_name)[key])
                        cpp_str += '    {' + str(key) + ',\"' + eval(obj_name)[key] + '\"},\n'
                    elif obj_name == 'data_str_key':
                        python_str += '    \'{}\' : {},\n'.format(key, eval(obj_name)[key])
                        cpp_str += '    {\"' + key + '\",' + str(eval(obj_name)[key]) + '},\n'
                python_str += '}\n\n'
                cpp_str += '};\n\n'
            cpp_str += '#endif /*{}_H*/'.format(obj_name.upper())

            if not os.path.isdir(python_dir):
                os.mkdir(python_dir)
            with open(os.path.join(python_dir, '{}.py'.format(filename)), 'w') as f:
                f.write(python_str)

            if not os.path.isdir(cpp_dir):
                os.mkdir(cpp_dir)
            with open(os.path.join(cpp_dir, '{}.h'.format(filename)), 'w') as f:
                f.write(cpp_str)

                    
        except:
            print('Failed to save {}'.format(filename))
            sys.exit(1)
    else:
        print('Updated version of pdf is available on {}'.format(pdf_url))

