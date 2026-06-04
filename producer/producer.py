import time
import random

from confluent_kafka import Producer
from faker import Faker

from shared.market_data_pb2 import Trade

fake = Faker()

conf = {
    'bootstrap.servers': 'localhost:9092',
    'batch.size': 1048576,
    'linger.ms': 5,
    'compression.type': 'lz4',
    'acks': '1'
}

producer = Producer(conf)

symbols = ["AAPL", "MSFT", "GOOG", "TSLA"]

print("Producer started...")

while True:

    trade = Trade()

    trade.exchange_id = "NASDAQ"
    trade.symbol = random.choice(symbols)
    trade.timestamp_ns = time.time_ns()
    trade.price = random.uniform(100, 500)
    trade.volume = random.randint(1, 1000)
    trade.trade_id = fake.uuid4()

    producer.produce(
        "raw-trades",
        value=trade.SerializeToString(),
        headers=[
            ("produce_time_ns", str(time.time_ns()).encode())
        ]
    )

    producer.poll(0)

    time.sleep(0.001)