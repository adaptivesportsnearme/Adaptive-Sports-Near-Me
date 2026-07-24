import requests
import re
import os
import win32pipe
import win32file
import win32con
from bs4 import BeautifulSoup
from urllib.parse import urljoin, urlparse
#"Bad" links are those we know do not have any need for the project, will update to keep it in a txt file for simplicity's sake
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
        
        return url == st_url;

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
            
            if response.status_code != 200:
                return
            
            #Filter out lines without any numbers in them
            soup = BeautifulSoup(response.text, 'html.parser')
            paragraphs = soup.find_all('p')
            filtered_paragraphs = [paragraph.get_text() for paragraph in paragraphs if re.search(r'\d', paragraph.get_text())]
            for p in filtered_paragraphs:
                print(p+'\n')

            def create_windows_fifo():
                # 1. Define the unique path in the Windows pipe filesystem
                pipe_name = r"\\.\pipe\mypythonfifo"

                print(f"Creating named pipe: {pipe_name}")

                # 2. Create the named pipe server handle
                # PIPE_ACCESS_OUTBOUND means this script will write data out to the pipe
                fifo_pipe = win32pipe.CreateNamedPipe(
                    pipe_name,
                    win32pipe.PIPE_ACCESS_OUTBOUND,
                    win32pipe.PIPE_TYPE_MESSAGE | win32pipe.PIPE_WAIT,
                    1, 65536, 65536,
                    0,
                    None
                )

                print("Waiting for client/reader process to connect...")
                # This call blocks execution until a client process opens the other side
                win32pipe.ConnectNamedPipe(fifo_pipe, None)
                print("Client connected! Sending data...")

                try:
                    # 3. Write data sequentially to the pipe
                    #for i in range(5):
                    #    message = f"Message batch {i}\n".encode('utf-8')
                    #    win32file.WriteFile(fifo_pipe, message)
                    #    print(f"Sent: Message batch {i}")
                    #    time.sleep(1) # Simulating a data stream
                    for p in filtered_paragraphs:
                        message = (p+'\n')
                        win32file.WriteFile(fifo_pipe, message)
                        time.sleep(1)

                finally:
                    # 4. Clean up and close the pipe handle
                    win32file.CloseHandle(fifo_pipe)
                    print("FIFO pipe closed.")

            if __name__ == "__main__":
                create_windows_fifo()

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

crawler = SimpleWebCrawler(start_url="https://www.challengedathletes.org/events/idaho-greenbelt-cycling/", max_depth=100)
st_url = "https://www.challengedathletes.org/events/idaho-greenbelt-cycling/"
crawler.crawl(crawler.start_url)
