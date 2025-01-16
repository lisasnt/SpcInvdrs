import zmq
import show_score_board_pb2

#context = zmq.Context()

# #  Socket to talk to server
# print("Connecting to server…")
# socket = context.socket(zmq.SUB)
# socket.connect("tcp://localhost:5555")

# #  Do 10 requests, waiting each time for a response
# for request in range(10):
#     print("Sending request %s …" % request)
#     socket.send(b"Hello")

#     #  Get the reply.
#     message = socket.recv()
#     print("Received reply %s [ %s ]" % (request, message))
def receive_score_board():
    context = zmq.Context()
    subscriber = context.socket(zmq.SUB)
    subscriber.connect("tcp://localhost:5555")  # Adjust the address as needed

    while True:
        try:
            message = subscriber.recv()
            print(f"Received raw message: {len(message)} bytes")
            score_board = show_score_board_pb2.ScoreBoard()
            score_board.ParseFromString(message)

            # Print the deserialized data
            print("Received Score Board:")
            for player, score in zip(score_board.players, score_board.scores):
                print(f"Astronaut {player}: {score} points")
        except Exception as e:
            print(f"Error: {e}")
    
if __name__ == "__main__":
    receive_score_board()