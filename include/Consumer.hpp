#ifndef MSC_CONSUMER_HPP
#define MSC_CONSUMER_HPP

#include "Exception.hpp"
#include "json.hpp"
#include "api/mediastreaminterface.h" // MediaStreamTrackInterface
#include <string>

namespace mediasoupclient
{
// Fast forward declarations.
class Transport;
class RecvTransport;

class Consumer
{
public:
	class Listener
	{
	public:
		virtual void OnClose(Consumer* consumer)                    = 0;
		virtual nlohmann::json OnGetStats(const Consumer* consumer) = 0;
	};

	/* Public Listener API */
	class PublicListener
	{
	public:
		virtual void OnTransportClose(Consumer* consumer) = 0;
	};

public:
	const std::string& GetId() const;
	const std::string& GetLocalId() const;
	const std::string& GetProducerId() const;
	const std::string GetKind() const;
	webrtc::MediaStreamTrackInterface* GetTrack() const;
	const nlohmann::json& GetRtpParameters() const;
	const nlohmann::json& GetAppData() const;
	nlohmann::json GetStats() const;

	bool IsClosed() const;
	bool IsPaused() const;

	void Close();
	void Pause();
	void Resume();

private:
	Consumer(
	  Listener* listener,
	  PublicListener* publicListener,
	  std::string id,
	  std::string localId,
	  std::string producerId,
	  webrtc::MediaStreamTrackInterface* track,
	  nlohmann::json rtpParameters,
	  nlohmann::json appData);

	void TransportClosed();

	/* RecvTransport will create instances and call private member TransporClosed */
	friend RecvTransport;

private:
	// Listener instance.
	Listener* listener;

	// Public listener instance.
	PublicListener* publicListener;

	// Id.
	std::string id;

	// localId.
	std::string localId;

	// Producer Id.
	std::string producerId;

	// Closed flag.
	bool closed{ false };

	// Paused flag.
	bool paused{ false };

	// Local track.
	webrtc::MediaStreamTrackInterface* track;

	// RTP parameters.
	nlohmann::json rtpParameters;

	// Max spatial layer.
	std::string maxSpatialLayer{};

	// App custom data.
	nlohmann::json appData{};
};

/* Inline methods */

inline const std::string& Consumer::GetId() const
{
	return this->id;
}

inline const std::string& Consumer::GetLocalId() const
{
	return this->localId;
}

inline const std::string& Consumer::GetProducerId() const
{
	return this->producerId;
}

inline const std::string Consumer::GetKind() const
{
	return this->track->kind();
}

inline webrtc::MediaStreamTrackInterface* Consumer::GetTrack() const
{
	return this->track;
}

inline const nlohmann::json& Consumer::GetRtpParameters() const
{
	return this->rtpParameters;
}

inline const nlohmann::json& Consumer::GetAppData() const
{
	return this->appData;
}

inline nlohmann::json Consumer::GetStats() const
{
	if (this->closed)
		throw Exception("Invalid state");
	else
		return this->listener->OnGetStats(this);
}

inline bool Consumer::IsClosed() const
{
	return this->closed;
}

inline bool Consumer::IsPaused() const
{
	return !this->track->enabled();
}
} // namespace mediasoupclient

#endif
