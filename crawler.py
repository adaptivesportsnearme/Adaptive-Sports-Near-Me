import requests
from bs4 import BeautifulSoup
firsturl = input("Enter first url: ")
def crawl(url, visited_urls=set()):
    if url in visited_urls:
        return
    visited_urls.add(url)
    with open('websites.txt','a') as file:
        file.write(url)
        file.write('\n')
    response = requests.get(url)
    
    next_page = soup.find('li', class_='next')
    if next_page:
        next_url = next_page.find('a')['href']
        crawl(url+next_url, visited_urls)

crawl(firsturl)
