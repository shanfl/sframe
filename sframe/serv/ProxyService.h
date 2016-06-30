
#ifndef SFRAME_PROXY_SERVICE_H
#define SFRAME_PROXY_SERVICE_H

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include "ServiceSession.h"
#include "Service.h"
#include "../util/Timer.h"
#include "../util/RingQueue.h"
#include "ProxyServiceMsg.h"

namespace sframe {

// ��������
class ProxyService : public Service
{
public:
	static const int kMaxSessionNumber = 1024;

public:

	ProxyService();

	virtual ~ProxyService();

	void Init() override;

	void OnDestroy() override;

	bool IsDestroyCompleted() const override;

	int32_t GetCyclePeriod() const override
	{
		return 1000;
	}

	// �������ڶ�ʱ��
	void OnCycleTimer() override;

	// �����ӵ���
	void OnNewConnection(const ListenAddress & listen_addr_info, const std::shared_ptr<sframe::TcpSocket> & sock) override;

	// ����������Ϣ
	void OnProxyServiceMessage(const std::shared_ptr<ProxyServiceMessage> & msg) override;

	// ע��Ự
	// ���ػỰID������0��������������Ϊʧ��
	int32_t RegistSession(int32_t sid, const std::string & remote_ip, uint16_t remote_port);

	// ע��Ự��ʱ��
	uint32_t RegistSessionTimer(int32_t session_id, int32_t after_ms, ObjectTimerManager<int32_t, ServiceSession>::TimerFunc func);

	// ��ȡ�Ự
	ServiceSession * GetServiceSessionById(int32_t session_id);

private:

	void OnMsg_SessionClosed(bool by_self, int32_t session_id);

	void OnMsg_SessionRecvData(int32_t session_id, const std::shared_ptr<std::vector<char>> & data);

	void OnMsg_SessionConnectCompleted(int32_t session_id, bool success);

private:
	ServiceSession * _session[kMaxSessionNumber + 1];
	std::unordered_map<int64_t, int32_t> _session_addr_to_sessionid;    // �����������ӵ�Session��Ŀ���ַ��sessionid��ӳ��
	RingQueue<int32_t, kMaxSessionNumber> _session_id_queue;
	int32_t _session_num;
	bool _listening;           // �Ƿ����ڼ���
	ObjectTimerManager<int32_t, ServiceSession> _timer_mgr;

	// Զ�̷����¼��Ϣ
	struct RemoteServiceInfo
	{
		int32_t sid;                                         // ����id
		int32_t sessionid;                                   // �����ỰID
		std::unordered_set<int32_t> linked_local_services;   // �������ı��ط���(��������Щ�����͹���Ϣ)
	};

	std::unordered_map<int32_t, RemoteServiceInfo> _remote_service_info;        // Զ�̷����¼��Ϣ
	std::unordered_map<int32_t, std::unordered_set<int32_t>> _sessionid_to_sid; // �ỰIDӳ�䵽����ID
};

}

#endif