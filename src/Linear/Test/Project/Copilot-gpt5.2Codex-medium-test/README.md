# Zuma - Stack/Queue/Linked List Demo

This project implements a simplified Zuma game using:
- linked list for the track,
- queue for upcoming balls,
- stack for undo snapshots.

## Run

```bash
./run.sh
```

Then open:

```
http://localhost:8080
```

## Controls
- Click a slot between balls to insert the next ball.
- Toggle Bomb Mode, then click a ball to remove it.
- Use Undo to revert the last action.

## Notes
- Win: the track becomes empty.
- Lose: the queue is empty and the track is not empty.
