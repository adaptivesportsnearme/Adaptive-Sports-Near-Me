import requests
from bs4 import BeautifulSoup
import pandas as pd
file_path = 'websites.txt'
line_to_delete = 1 #Going for first line in website list

with open(file_path,'r') as file:
    first_line = file.readline().strip()

url = first_line
response = requests.get(url)
soup = BeautifulSoup(response.text, 'html.parser')
for script_or_style in soup(['script','style']):
    script_or_style.decompose()
all_text = soup.get_tezxt(sparator='\n')
cleaned_lines = [line.strip() for line in all_text.splitlines() if line.strip()]
final_output = '\n'.join(cleaned_lines)
print(final_output) # This will be modified to get data we want, used just for testing purposes for the moment

with open("file.txt", "r+") as file:
    file.readline()
    remaining_data = file.read()
    file.seek(0)
    file.write(remaining_data)
    file.truncate()