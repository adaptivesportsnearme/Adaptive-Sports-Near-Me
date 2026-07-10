import requests
from bs4 import BeautifulSoup

#Gets information from txt file
with open('handoff.txt', 'r') as file:
    for line in file:
        url = line

# Extract most likely useful information
response = requests.get(url)
html_content = response.text
soup = BeautifulSoup(html_content, 'html.parser')
potential=soup.find_all('p')
for p in potential:
    print(p)
