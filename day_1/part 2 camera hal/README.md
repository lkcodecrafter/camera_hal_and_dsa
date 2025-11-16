```============================
🟩 PART 2 — CAMERA HAL: HAL3 Architecture
============================

Let’s build the mental model that Google expects you to know.

⭐ HAL3 is a pipeline of 3 main components
App -> Framework -> CameraService -> CameraProvider -> HAL3 -> ISP -> Sensor

Simple Breakdown

App (CameraX/Camera2)
Sends capture requests.

Camera Framework
Converts high-level API → lower internal calls.

CameraService
Central manager:

Permissions

Connect apps

Multi-client handling

Routing to HAL

CameraProvider
Vendor-side module registration

Camera HAL3
Your area!
Implements:

processCaptureRequest()

allocate buffers

configure streams

fills metadata

talks to ISP

ISP (Image Signal Processor)
Low-level camera processing:

AWB

AF

AE

Denoise

Sharpen

RAW → YUV

Sensor
Camera hardware.

⭐ HAL3 Main Functions You Must Know
1. open()

– Open camera
– Allocate resources

2. configure_streams()

– Configure preview / still / video streams
– Decide resolutions, formats

3. process_capture_request()

MOST IMPORTANT.
Google always asks about this.

Inside process_capture_request():

Read metadata → set exposure/focus

Queue request to ISP

Fill result metadata

Return buffers

4. flush()

Cancel ongoing requests.

⭐ Today's Camera Interview Questions

(You try to answer later; I will evaluate)

Q1: Explain HAL3 capture pipeline from request to result.
Q2: What happens inside process_capture_request()?
Q3: How does HAL3 interact with the ISP?

I can help refine your answers anytime.