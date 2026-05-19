#!/usr/bin/env python3
"""
HTTP Client Coredump Reproduction Script

This script reproduces the coredump issue by triggering rapid client destruction
and shutdown scenarios from the client side. It sends requests to BMCWeb and
then abruptly terminates connections to trigger use-after-free conditions.

Usage:
    python3 test_http_client_coredump.py --host <bmcweb_host> --port <port>
    
Example:
    python3 test_http_client_coredump.py --host 192.168.1.100 --port 443
"""

import argparse
import requests
import threading
import time
import sys
import signal
import urllib3
from concurrent.futures import ThreadPoolExecutor

# Suppress SSL warnings for testing
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)


class CoredumpReproducer:
    def __init__(self, host, port, use_ssl=True):
        self.host = host
        self.port = port
        self.protocol = "https" if use_ssl else "http"
        self.base_url = f"{self.protocol}://{host}:{port}"
        self.session = None
        self.stop_flag = threading.Event()
        
    def test_rapid_destruction(self, iterations=100):
        """
        Method 1: Rapid Client Destruction
        Creates and destroys sessions rapidly while requests are in flight.
        """
        print(f"\n[TEST 1] Rapid Client Destruction ({iterations} iterations)")
        print("=" * 60)
        
        for i in range(iterations):
            # Create new session
            session = requests.Session()
            session.verify = False
            
            # Send request to a slow endpoint or non-existent resource
            try:
                # Use a thread to send request
                def send_request():
                    try:
                        session.post(
                            f"{self.base_url}/redfish/v1/EventService/Subscriptions",
                            json={
                                'Destination': 'http://192.0.2.1:8080/events',
                                'Protocol': 'Redfish'
                            },
                            timeout=30
                        )
                    except Exception as e:
                        pass  # Expected to fail
                
                thread = threading.Thread(target=send_request)
                thread.daemon = True
                thread.start()
                
                # Destroy session immediately while request is pending
                time.sleep(0.01)  # 10ms delay
                session.close()
                del session
                
                if (i + 1) % 10 == 0:
                    print(f"  Completed {i + 1}/{iterations} iterations")
                    
            except Exception as e:
                print(f"  Error in iteration {i}: {e}")
        
        print("✓ Test 1 complete")
    
    def test_shutdown_during_requests(self, num_requests=50):
        """
        Method 2: Shutdown During Active Requests
        Sends multiple concurrent requests and abruptly closes all connections.
        """
        print(f"\n[TEST 2] Shutdown During Active Requests ({num_requests} requests)")
        print("=" * 60)
        
        session = requests.Session()
        session.verify = False
        
        # URLs to test - mix of valid and invalid endpoints
        urls = [
            f"{self.base_url}/redfish/v1/EventService/Subscriptions",
            f"{self.base_url}/redfish/v1/Systems",
            f"{self.base_url}/redfish/v1/Managers",
            f"{self.base_url}/redfish/v1/Chassis",
            f"{self.base_url}/redfish/v1/AccountService",
        ]
        
        threads = []
        
        def send_request(url, index):
            try:
                session.post(
                    url,
                    json={'Destination': f'http://192.0.2.{index}:8080/events'},
                    timeout=30
                )
            except Exception:
                pass  # Expected to fail
        
        # Start all requests
        for i in range(num_requests):
            url = urls[i % len(urls)]
            thread = threading.Thread(target=send_request, args=(url, i))
            thread.daemon = True
            threads.append(thread)
            thread.start()
        
        # Wait briefly for requests to be in various states
        time.sleep(0.05)
        
        # Abruptly close session while requests are pending
        print("  Closing session while requests are in flight...")
        session.close()
        del session
        
        print("✓ Test 2 complete")
    
    def test_connection_pool_stress(self, num_requests=100):
        """
        Method 3: Connection Pool Stress Test
        Overwhelms the connection pool with many concurrent requests.
        """
        print(f"\n[TEST 3] Connection Pool Stress Test ({num_requests} requests)")
        print("=" * 60)
        
        session = requests.Session()
        session.verify = False
        
        # Configure session for high concurrency
        from requests.adapters import HTTPAdapter
        adapter = HTTPAdapter(
            pool_connections=10,
            pool_maxsize=10,
            max_retries=3
        )
        session.mount('http://', adapter)
        session.mount('https://', adapter)
        
        with ThreadPoolExecutor(max_workers=20) as executor:
            futures = []
            
            for i in range(num_requests):
                future = executor.submit(
                    self._send_subscription_request,
                    session,
                    i
                )
                futures.append(future)
            
            # Wait briefly then destroy session
            time.sleep(0.02)
            print("  Destroying session while pool is active...")
            session.close()
            
            # Try to collect results (will mostly fail)
            completed = 0
            failed = 0
            for future in futures:
                try:
                    future.result(timeout=0.1)
                    completed += 1
                except Exception:
                    failed += 1
            
            print(f"  Completed: {completed}, Failed: {failed}/{num_requests}")
        
        print("✓ Test 3 complete")
    
    def test_timeout_race_condition(self):
        """
        Method 4: Timer Race Condition
        Triggers timeout while destroying connection.
        """
        print(f"\n[TEST 4] Timer Race Condition")
        print("=" * 60)
        
        session = requests.Session()
        session.verify = False
        
        def send_slow_request():
            try:
                # Send request with short timeout to non-routable IP
                session.post(
                    f"{self.base_url}/redfish/v1/EventService/Subscriptions",
                    json={'Destination': 'http://192.0.2.1:8080/events'},
                    timeout=2  # Short timeout
                )
            except requests.exceptions.Timeout:
                print("  Timeout occurred (expected)")
            except Exception as e:
                print(f"  Exception: {type(e).__name__}")
        
        thread = threading.Thread(target=send_slow_request)
        thread.daemon = True
        thread.start()
        
        # Wait for timeout to be close to expiring
        time.sleep(1.9)
        
        # Destroy session just before timeout fires
        print("  Destroying session near timeout...")
        session.close()
        del session
        
        # Wait for timeout to fire
        time.sleep(0.5)
        
        print("✓ Test 4 complete")
    
    def test_event_subscription_lifecycle(self, num_cycles=20):
        """
        Method 5: Real-World Scenario - Event Subscription Lifecycle
        Simulates creating and deleting event subscriptions rapidly.
        """
        print(f"\n[TEST 5] Event Subscription Lifecycle ({num_cycles} cycles)")
        print("=" * 60)
        
        for i in range(num_cycles):
            session = requests.Session()
            session.verify = False
            
            try:
                # Create subscription
                response = session.post(
                    f"{self.base_url}/redfish/v1/EventService/Subscriptions",
                    json={
                        'Destination': f'http://192.0.2.{i}:8080/events',
                        'Protocol': 'Redfish',
                        'Context': f'test-{i}'
                    },
                    timeout=5,
                    auth=('root', '0penBmc')  # Default credentials
                )
                
                if response.status_code == 201:
                    # Get subscription location
                    location = response.headers.get('Location', '')
                    
                    # Immediately delete subscription (triggers cleanup)
                    if location:
                        session.delete(
                            f"{self.base_url}{location}",
                            timeout=1,
                            auth=('root', '0penBmc')
                        )
                
            except Exception as e:
                pass  # Expected failures
            finally:
                # Abruptly close session
                session.close()
                del session
            
            if (i + 1) % 5 == 0:
                print(f"  Completed {i + 1}/{num_cycles} cycles")
        
        print("✓ Test 5 complete")
    
    def _send_subscription_request(self, session, index):
        """Helper method to send a subscription request"""
        try:
            session.post(
                f"{self.base_url}/redfish/v1/EventService/Subscriptions",
                json={
                    'Destination': f'http://192.0.2.{index % 255}:8080/events',
                    'Protocol': 'Redfish'
                },
                timeout=5
            )
        except Exception:
            pass  # Expected to fail
    
    def run_all_tests(self):
        """Run all reproduction tests"""
        print("\n" + "=" * 60)
        print("HTTP Client Coredump Reproduction Tests")
        print(f"Target: {self.base_url}")
        print("=" * 60)
        
        try:
            self.test_rapid_destruction(iterations=50)
            time.sleep(1)
            
            self.test_shutdown_during_requests(num_requests=30)
            time.sleep(1)
            
            self.test_connection_pool_stress(num_requests=50)
            time.sleep(1)
            
            self.test_timeout_race_condition()
            time.sleep(1)
            
            self.test_event_subscription_lifecycle(num_cycles=10)
            
            print("\n" + "=" * 60)
            print("All tests completed!")
            print("Check BMCWeb logs for crashes or coredumps")
            print("=" * 60)
            
        except KeyboardInterrupt:
            print("\n\nTests interrupted by user")
            sys.exit(0)


def main():
    parser = argparse.ArgumentParser(
        description='Reproduce HTTP Client coredump issues in BMCWeb'
    )
    parser.add_argument(
        '--host',
        required=True,
        help='BMCWeb host address (e.g., 192.168.1.100)'
    )
    parser.add_argument(
        '--port',
        type=int,
        default=443,
        help='BMCWeb port (default: 443)'
    )
    parser.add_argument(
        '--no-ssl',
        action='store_true',
        help='Use HTTP instead of HTTPS'
    )
    parser.add_argument(
        '--test',
        type=int,
        choices=[1, 2, 3, 4, 5],
        help='Run specific test only (1-5)'
    )
    
    args = parser.parse_args()
    
    reproducer = CoredumpReproducer(
        host=args.host,
        port=args.port,
        use_ssl=not args.no_ssl
    )
    
    if args.test:
        # Run specific test
        test_methods = {
            1: reproducer.test_rapid_destruction,
            2: reproducer.test_shutdown_during_requests,
            3: reproducer.test_connection_pool_stress,
            4: reproducer.test_timeout_race_condition,
            5: reproducer.test_event_subscription_lifecycle
        }
        test_methods[args.test]()
    else:
        # Run all tests
        reproducer.run_all_tests()


if __name__ == '__main__':
    main()

# Made with Bob
