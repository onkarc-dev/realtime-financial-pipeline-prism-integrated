import time
import redis
import uuid

from confluent_kafka import Consumer, KafkaError

from shared.market_data_pb2 import Trade


# Redis connection
redis_client = redis.Redis(
    host='localhost',
    port=6379,
    decode_responses=True
)


# Kafka consumer config
conf = {
    'bootstrap.servers': 'localhost:9092',
    'group.id': f'trade-group-{uuid.uuid4()}',
    'auto.offset.reset': 'latest'
}


consumer = Consumer(conf)

consumer.subscribe(['raw-trades'])

print("Consumer started...")


# Throughput tracking
message_count = 0
start_time = time.time()


try:

    while True:

        msg = consumer.poll(1.0)

        if msg is None:
            continue

        # Kafka error handling
        if msg.error():

            if msg.error().code() == KafkaError._PARTITION_EOF:
                continue
            else:
                print(f"Kafka Error: {msg.error()}")
                continue

        # Deserialize protobuf message
        trade = Trade()
        trade.ParseFromString(msg.value())

        # Read headers safely
        headers = dict(msg.headers() or {})

        produce_time = headers.get('produce_time_ns')

        # Latency calculation
        if produce_time is not None:

            produce_time_ns = int(produce_time)

            latency_ms = (
                time.time_ns() - produce_time_ns
            ) / 1_000_000

        else:
            latency_ms = -1

        # Store latest price in Redis
        redis_client.set(
            trade.symbol,
            trade.price
        )

        # Throughput tracking
        message_count += 1

        elapsed = time.time() - start_time

        if elapsed >= 1:

            throughput = message_count / elapsed

            print(
                f"\nThroughput: {throughput:.2f} msg/sec\n"
            )

            message_count = 0
            start_time = time.time()

        # Output
        print(
            f"{trade.symbol} | "
            f"Price={trade.price:.2f} | "
            f"Volume={trade.volume} | "
            f"Latency={latency_ms:.2f} ms"
        )


except KeyboardInterrupt:

    print("\nShutting down consumer...")


finally:

    consumer.close()

    print("Consumer closed.")