
#include <string>
#include <stdio.h>
#include <iostream>
#include <queue>
#include <algorithm>
#include "sched.h"

class FCFS : public Scheduler
{
private:
    /*
    * 구현 (멤버 변수/함수 추가 및 삭제 가능)
    */

public:
    FCFS(std::queue<Job> jobs, double switch_overhead) : Scheduler(jobs, switch_overhead)
    {
        name = "FCFS";
    }

    int run() override
    {
          //1. 현재 CPU가 비어 있으면 새로 실행할 작업을 가져온다
        if (current_job_.name == 0) {
            
            // 모든 대기 작업이 완료되어 큐가 비어있다면 -1 반환한다
            if (job_queue_.empty()) {
                return -1;
            }
            
            // 대기 큐의 맨 앞(가장 먼저 도착한) 작업을 꺼내 CPU에 할당한다
            current_job_ = job_queue_.front();
            job_queue_.pop();
            
            // 문맥 교환 오버헤드 반영: 시스템 시작 시점이 아닐 때만 실행 시간을 추가한다
            if(current_time_ != 0) { current_time_ += switch_time_;}
            
            // 이 작업이 처음 실행되는 시간이므로 first_run_time 기록한다
            current_job_.first_run_time = current_time_;
        }

        // 2. 현재 실행 중인 작업의 정보를 저장한다
        int running_job_name = current_job_.name;

        //3. 1초 동안 작업 실행 (남은 시간 줄이고 전체 시간 늘린다)
        current_job_.remain_time -= 1;  // 해당 작업의 남은 실행 시간 감소
        current_time_ += 1;            // 시스템 전체 흐름 시간 증가

        // 4. 작업 완료 검사 및 후처리
        // 남은 실행 시간이 0 이하가 되면 해당 작업은 수행이 종료된 것이다
        if (current_job_.remain_time <= 0) {
            // 완료 시간 기록
            current_job_.completion_time = current_time_;
            
            // 완료된 작업 목록에 저장 순서대로 push_back
            end_jobs_.push_back(current_job_);
            
            // 다음 1초(다음 run 호출)에 새로운 작업을 꺼낼 수 있도록 CPU를 비운다
            current_job_.name = 0;
        }

        // 5. 지금 막 실행한 것의 이름을 리턴해서 기록한다
        return running_job_name;

        
    }
};

class SPN : public Scheduler
{
private:
    
	std::vector<Job> ready_queue; // 도착한 작업들을 모아두고 비교할 대기실

public:
    SPN(std::queue<Job> jobs, double switch_overhead) : Scheduler(jobs, switch_overhead)
    {
        
	name = "SPN"; // 알고리즘 이름 설정
    }

    int run() override
    {
       
        // 1. 현재 시스템 시간(current_time_) 이전에 도착한 작업을 
        // 전체 작업 큐(job_queue_)에서 꺼내 대기실(ready_queue)로 이동시킨다
        while (!job_queue_.empty() && job_queue_.front().arrival_time <= current_time_) {
            ready_queue.push_back(job_queue_.front());
            job_queue_.pop();
        }

        // 2. 현재 CPU가 비어있다면 새로운 작업을 골라야 한다
        if (current_job_.name == 0) {
            
            // 만약 대기실도 비었고 남은 작업도 없으면 모든 시뮬레이션을 종료한다
            if (ready_queue.empty() && job_queue_.empty()) {
                return -1;
            }

            // 대기실은 비었는데 아직 도착 안 한 미래 작업이 큐에 있는 경우
            if (ready_queue.empty() && !job_queue_.empty()) {
                ready_queue.push_back(job_queue_.front());
                job_queue_.pop();
                // 작업 도착 시간까지 시스템 시간을 강제로 점프시킨다 
                if (current_time_ < ready_queue.back().arrival_time) {
                    current_time_ = ready_queue.back().arrival_time;
                }
            }

            // 대기실에서 service_time이 가장 짧은 것을 찾는다
            auto shortest = ready_queue.begin();
            for (auto it = ready_queue.begin(); it != ready_queue.end(); ++it) {
                // 더 짧은 거 발견 시 갱신한다
                if (it->service_time < shortest->service_time) {
                    shortest = it;
                } 
                // 실행 시간이 똑같다면 먼저 도착한 것을 우선으로 한다
                else if (it->service_time == shortest->service_time) {
                    if (it->arrival_time < shortest->arrival_time) {
                        shortest = it;
                    }
                }
            }

            // 찾아낸 가장 짧은 작업을 CPU에 할당하고 대기실에서 뺀다
            current_job_ = *shortest;
            ready_queue.erase(shortest);

            // 문맥 교환 오버헤드 반영: 최초 실행(0초)이 아닐 경우에만 시간 추가한다
            if (current_time_ != 0) {
                current_time_ += switch_time_;
            }
            
            // 이 작업의 첫 실행 시간을 기록한다
            current_job_.first_run_time = current_time_;
        }

        // 3. 작업 1초 실행 처리한다
        int running_job_name = current_job_.name;
        current_job_.remain_time -= 1;
        current_time_ += 1;

        // 4. 방금 1초를 끝으로 작업이 완료되었다면
        if (current_job_.remain_time <= 0) {
            current_job_.completion_time = current_time_; // 끝난 시간 기록
            end_jobs_.push_back(current_job_);            // 완료 목록에 추가
            current_job_.name = 0;                        // CPU 자리 비우기
        }

        // 5. 이번 턴에 실행한 것 이름을 반환한다
        return running_job_name;


    }
};

class RR : public Scheduler
{
private:
    int time_slice_;
    int left_slice_;
    std::queue<Job> waiting_queue;


public:
    RR(std::queue<Job> jobs, double switch_overhead, int time_slice) : Scheduler(jobs, switch_overhead)
    {
        name = "RR_" + std::to_string(time_slice);
        /*
         * 위 생성자 선언 및 이름 초기화 코드 수정하지 말것.
         * 나머지는 자유롭게 수정 및 작성 가능 (아래 코드 수정 및 삭제 가능)
         */
        time_slice_ = time_slice;
        left_slice_ = time_slice;
    }

    int run() override
    {
   
         // 1. 할당된 작업이 없고, job_queue가 비어있지 않으면 
        //job큐의 맨앞에 있는 job을 작업 현재 작업으로 할당 및 기존 맨앞을 pop한다
        if (current_job_.name == 0 && !job_queue_.empty()) {
            current_job_ = job_queue_.front();
            job_queue_.pop();
        }

        // 2.job큐가 비어있지 않고, job큐의 맨앞에 있는 job의 도착시간이 현재시간보다 작거나 같으면
        // job큐의 맨앞에 있는 job을 대기큐에 넣고, job큐에서 pop 한다
        //도착한 작업을 waiting_queue로 이동한다 (하나씩)
        if (!job_queue_.empty() && job_queue_.front().arrival_time <= current_time_) {
            // 대기 큐에 작업 추가
            waiting_queue.push(job_queue_.front()); 
            // 대기 큐에 추가한 작업은 job_queue에서 제거
            job_queue_.pop();
        }

        // 3. 현재 작업의 서비스시간을 모두 채워서 job자체가 완료된 것이면, 
        //완료시간기록, 완료벡터에 저장, 남은 타임슬라이스 초기화(다음작업대비), 남은작업없으면 종료, 
        // 남은작업 있으면 레디큐에서 새로운 작업 꺼내서 current_job에 할당 후 context switch 타임에 추가한다
        if (current_job_.remain_time == 0) {
            // 작업 완료 시간 기록
            current_job_.completion_time = current_time_;
            // 작업 완료 벡터에 저장
            end_jobs_.push_back(current_job_);

            left_slice_ = time_slice_;  // 다음 작업 대비 초기화

            // 남은 작업이 없으면 종료
            if (waiting_queue.empty()) return -1;

            // 새로운 작업 할당
            current_job_ = waiting_queue.front();
            waiting_queue.pop();
            // context switch 타임 추가
            current_time_ += switch_time_;
        }

        // 4. 타임 슬라이스(1초) 다 썼을 때, 다른 대기 작업이 있으면 순서 교체한다
        if (left_slice_ == 0) {
            if (!waiting_queue.empty()) { //대기 큐가 비어있는게 아니라면, 다음 작업이 있다면 
                // 현재 작업을 대기 큐에 넣고, 대기 큐에서 다음 작업을 가져온다
                waiting_queue.push(current_job_);
                current_job_ = waiting_queue.front();
                // 대기 큐에서 (방금 할당한) 작업 제거
                waiting_queue.pop();
                // context switch 타임 추가
                current_time_ += switch_time_;
            }
            //다음 작업을 할당했으니, 타임슬라이스 초기화
            left_slice_ = time_slice_;  
        }

        // 5. 현재 작업이 처음 스케줄링 되는 것이라면
        if (current_job_.service_time == current_job_.remain_time) {
            // 첫 실행 시간 기록
            current_job_.first_run_time = current_time_;
        }

        // 6. 1초 지날때마다 남은 타임슬라이스를 줄여준다
        left_slice_--;
        // 현재 시간 ++
        current_time_++;
        // 작업의 남은 시간 --
        current_job_.remain_time--;

        // 7. 현재 실행 중인 작업의 이름 반환
        return current_job_.name;



    }
};


// FeedBack 스케줄러 (queue 개수 : 4 / boosting 없음)
class FeedBack : public Scheduler
{
private:

    std::queue<Job> queue[4]; // 각 요소가 하나의 큐인 배열 선언
    int quantum[4] = {1, 1, 1, 1}; // 각 큐에 대한 시간 슬라이스 초기화
    int left_slice_; // 현재 작업의 남은 시간 슬라이스
    int current_queue; // 현재 큐의 인덱스

public:
    FeedBack(std::queue<Job> jobs, double switch_overhead, bool is_2i) : Scheduler(jobs, switch_overhead) {
        if (is_2i) {
            name = "FeedBack_2i";
        }
        else {
            name = "FeedBack_1";
        }
        /*
         * 위 생성자 선언 및 이름 초기화 코드 수정하지 말것.
         * 나머지는 자유롭게 수정 및 작성 가능
         */
         // 아래쪽 큐로 갈수록 타임 슬라이스를 2배씩 늘려준다
         if (name == "FeedBack_2i") {
             quantum[0] = 1;
             quantum[1] = 2;
             quantum[2] = 4;
             quantum[3] = 8;
         }
    }

    int run() override {
  
            // 1. 큐에 도달한 작업을 첫 번째 큐에 추가한다
            if (!job_queue_.empty() && job_queue_.front().arrival_time <= current_time_) {
                queue[0].push(job_queue_.front());
                job_queue_.pop();
            }
    
            // 2. 할당된 작업이 없고 큐가 비어있지 않으면 작업을 할당한다
            if (current_job_.name == 0 && (!queue[0].empty() || !queue[1].empty() || !queue[2].empty() || !queue[3].empty())) {
                current_job_ = queue[0].front(); // 첫 번째 큐에서 시작
                queue[0].pop();
                left_slice_ = quantum[0]; // 첫 번째 큐의 시간 슬라이스로 설정
                current_queue = 0; // 큐 인덱스 기록
            }
    
            // 3. 현재 작업의 남은 시간이 0이면 작업이 완료된다
            if (current_job_.remain_time == 0) {
                current_job_.completion_time = current_time_;
                end_jobs_.push_back(current_job_); // 완료된 작업 목록에 추가
    
                // 모든 큐가 비어있으면 종료한다
                if (job_queue_.empty() && (queue[0].empty() && queue[1].empty() && queue[2].empty() && queue[3].empty())) {
                    return -1;
                }
    
                // 새 작업을 할당한다
                for (int i = 0; i < 4; i++) {
                    if (!queue[i].empty()) {
                        current_job_ = queue[i].front();
                        left_slice_ = quantum[i];
                        queue[i].pop();
                        current_queue = i;
                        break;
                    }
                }
                current_time_ += switch_time_; // 문맥 교환 시간 추가
            }
    
            // 4. 할당량 다 채우면 한 단계 낮은 우선순위 큐로 강등 시킨다
            if (left_slice_ == 0) {
                if (current_queue == 3 || (queue[0].empty() && queue[1].empty() && queue[2].empty() && queue[3].empty())) {
                    queue[current_queue].push(current_job_); // 가장 낮은 우선순위 큐라면 그대로 넣기
                } else {
                    queue[current_queue + 1].push(current_job_); // 우선순위 낮은 큐로 이동
                }
    
                // 비어 있지 않은 큐를 찾아 작업을 할당한다
                for (int i = 0; i < 4; i++) {
                    if (!queue[i].empty()) {
                        if (current_job_.name != queue[i].front().name) { // 문맥 교환 발생
                            current_time_ += switch_time_;
                        }
                        current_job_ = queue[i].front();
                        left_slice_ = quantum[i];
                        queue[i].pop();
                        current_queue = i;
                        break;
                    }
                }
            }
    
            // 5. 첫 실행 시 첫 실행 시간 기록한다
            if (current_job_.service_time == current_job_.remain_time) {
                current_job_.first_run_time = current_time_;
            }
    
            // 현재 시간과 작업의 남은 시간 갱신한다
            current_time_++;
            current_job_.remain_time--;
            left_slice_--; // 남은 타임슬라이스 갱신
    
            // 현재 작업의 이름을 반환한다
            return current_job_.name;
        

    }

};
