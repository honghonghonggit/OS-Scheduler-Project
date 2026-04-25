
#include <string>
#include <stdio.h>
#include <iostream>
#include <queue>
#include <algorithm>
#include <random>
#include <unordered_map>
#include "sched.h"


class Lottery : public Scheduler{
    private:
        int counter = 0;
        int total_tickets = 0;
        int winner = 0;
        std::mt19937 gen;  // 난수 생성기
	int previousJob = -1;  //추가한 변수는 이전 작업 이름
        
    public:
        Lottery(std::list<Job> jobs, double switch_overhead) : Scheduler(jobs, switch_overhead) {
            name = "Lottery";
            // 난수 생성기 초기화
            uint seed = 10; // seed 값 수정 금지
            gen = std::mt19937(seed);
            total_tickets = 0;
            for (const auto& job : job_list_) {
                total_tickets += job.tickets;
            }
        }

        int getRandomNumber(int min, int max) {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(gen);
        }

        int run() override {
        //1. 아직 안 끝난 작업들 티켓만 다시 다 더한다
        bool allJobsDone = true;    // 모든 작업이 완료되었는지 확인하는 변수
        total_tickets = 0;

        for (const auto& job : job_list_) {
            if (job.remain_time > 0) {
                allJobsDone = false;
                total_tickets += job.tickets;
            }
        }
        // 모든 작업의 남은 시간이 0이면 시뮬레이션을 종료한다
        if (allJobsDone) {
            return -1;
        }
        //2. 0부터 티켓 수 사이에서 당첨 번호 하나를 뽑는다
        winner = getRandomNumber(0, total_tickets - 1);
        counter = 0;
        //3. 티켓 누적으로 당첨된 작업 찾는다
        auto it = std::find_if(job_list_.begin(), job_list_.end(), [&](Job& job) {
            if (job.remain_time > 0) {
                counter += job.tickets;
                return counter > winner;
            }
            return false;
        });
        //4. 당첨된 작업을 실행 처리한다
        if (it != job_list_.end()) {
            // 이전이랑 다른 작업이 뽑혔으면 문맥 교환 시간을 더한다
            if (previousJob != -1 && previousJob != it->name) {
                current_time_ += switch_time_;
            }

            previousJob = it->name;  // 이번 작업 이름 저장

            if (it->service_time == it->remain_time) {
                it->first_run_time = current_time_;
            }
            // 1초 일 시킨다
            current_time_++;
            it->remain_time--; //뽑힌 것 1초 일 시킨다
            // 일 다 끝냈으면 종료 시간 기록하고 완료 목록으로 보낸다
            if (it->remain_time == 0) {
                it->completion_time = current_time_;
                end_jobs_.push_back(*it);
            }

            return it->name; //실행한 것 이름 리턴
        }

            return -1;
        }
};


class Stride : public Scheduler{
    private:
        // 각 작업의 현재 pass 값과 stride 값을 관리하는 맵
        std::unordered_map<int, int> pass_map;  
        std::unordered_map<int, int> stride_map;  
        const int BIG_NUMBER = 10000; // stride 계산을 위한 상수 
	int previousJobName = -1;

    public:
        Stride(std::list<Job> jobs, double switch_overhead) : Scheduler(jobs, switch_overhead) {
            name = "Stride";
                    
            for (auto &job : job_list_) {
                // stride = BIG_NUMBER / tickets (tickets는 0이 아님을 가정)
                stride_map[job.name] = BIG_NUMBER / job.tickets;
                pass_map[job.name] = 0; // 처음엔 다 같이 0에서 시작
            }
        }

        int run() override {
           //1. 모든 작업이 끝났는지 체크한다
            bool areAllJobsDone = std::all_of(job_list_.begin(), job_list_.end(), [](const Job& job) {
                return job.remain_time <= 0;
            });
    
            if (areAllJobsDone) {
                return -1;
            }

            // 2. 가장 적게 달린(pass 값이 가장 작은) 작업을 찾는다
            auto selectedJobIt = std::min_element(job_list_.begin(), job_list_.end(), [&](const Job& a, const Job& b) {
                if (a.remain_time <= 0) return false;
                if (b.remain_time <= 0) return true;
                return pass_map[a.name] < pass_map[b.name];
            });

            // 3. 선택된 작업을 실행한다
            if (selectedJobIt != job_list_.end()) {
                Job& selectedJob = *selectedJobIt;
    
                // 다른 놈으로 교체되면 문맥 교환 시간 페널티
                if (previousJobName != -1 && previousJobName != selectedJob.name) {
                    current_time_ += switch_time_;
                }
    
                // 처음 실행될 때 시작 시간 기록
                if (selectedJob.service_time == selectedJob.remain_time) {
                    selectedJob.first_run_time = current_time_;
                }
    
                // 1초 동안 일 시킴
                current_time_++;
                selectedJob.remain_time--;

                // 일한 만큼 pass 값을 stride만큼 증가시킴
                pass_map[selectedJob.name] += stride_map[selectedJob.name];
    
                // 작업 끝났으면 정보 정리해서 완료 목록에 저장
                if (selectedJob.remain_time == 0) {
                    selectedJob.completion_time = current_time_;
                    end_jobs_.push_back(selectedJob);
                }
    
                previousJobName = selectedJob.name; // 이번에 일한 놈 저장
                return selectedJob.name;
            }


            return -1;
        }
};
