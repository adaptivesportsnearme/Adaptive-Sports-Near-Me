import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin, urlparse
#"Bad" links are those we know do not have any need for the project, will update to keep it in a txt file for simplicity's sake
bad_links = ["www.donordrive.com", "www.eosfitness.com", "www.youtube.com", "eosfitness.com", "www.ossur.com"]
#Max_depth is kept for testing purposes, do not want the crawler to get out of hand yet.
class SimpleWebCrawler:
    def __init__(self, start_url, max_depth=2):
        self.start_url = start_url
        self.max_depth = max_depth
        self.visited_urls = set()
        self.base_domain = urlparse(start_url).netloc

    def is_valid(self, url):
        parsed = urlparse(url)
        #same_domain = parsed.netloc == self.base_domain or parsed.netloc == ""
        not_bad = True
        for b in bad_links:
            if parsed.hostname == b:
                not_bad = False
                print(f"{not_bad}")
        not_asset = not parsed.path.endswith(('.pdf', '.jpg', '.png', '.gif', '.zip'))
        
        return not_bad and not_asset

    def crawl(self, current_url, current_depth=0):
        if current_depth > self.max_depth or current_url in self.visited_urls:
            return

        print(f"[{current_depth}] Crawling: {current_url}")

        self.visited_urls.add(current_url)
        with open("handoff.txt","a") as file:
            file.write(f"{current_url}\n")

        try:
            headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64)'}
            response = requests.get(current_url, headers=headers, timeout=5)
            response.raise_for_status()
            print(f"Raw response: {response.text}")
            data = response.json()
            print(data)
            if response.status_code != 200:
                return
            
            
            soup = BeautifulSoup(response.text, 'html.parser')
            
            # Find all hyperlinks
            for anchor in soup.find_all('a', href=True):
                href = anchor['href']
                full_url = urljoin(current_url, href)
                
                if self.is_valid(full_url):
                    clean_url = full_url.split('#')[0]
                    if clean_url not in self.visited_urls:
                        self.crawl(clean_url, current_depth + 1)

        except requests.exceptions.RequestException as e:
            print(f"Error crawling {current_url}: {e}")

crawler = SimpleWebCrawler(start_url="https://www.challengedathletes.org/", max_depth=100)
crawler.crawl(crawler.start_url)
