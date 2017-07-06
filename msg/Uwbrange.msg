Header              header
uint8               requester_id
uint8               responder_id            # Identity number of the anchor node
float32             distance                # Distance measurement
float32             distance_err            # Distance error
uint8               antenna                 # Antenna where the measurement was carried out, 0 by default
geometry_msgs/Point responder_location      # Location of the responding node if known.