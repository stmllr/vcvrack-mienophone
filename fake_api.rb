require 'webrick'
require 'json'

def random_result
    [  
        {  
            faceAttributes: { 
                emotion: {
                    anger: rand.round(3),
                    contempt: rand.round(3),
                    disgust: rand.round(3),
                    fear: rand.round(3),
                    happiness: rand.round(3),
                    neutral: rand.round(3),
                    sadness: rand.round(3),
                    surprise: rand.round(3),
                }
            }
        }
    ]
end

server = WEBrick::HTTPServer.new :Port => 8080

trap 'INT' do
    server.shutdown
end

server.mount_proc '/face/v1.0/detect' do |req, res|
    res['Content-Type'] = 'application/json'
    res.body = random_result.to_json
end

server.start
